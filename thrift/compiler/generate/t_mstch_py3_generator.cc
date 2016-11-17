/*
 * Copyright 2016 Facebook, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <thrift/compiler/generate/t_mstch_generator.h>

#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

namespace {

class t_mstch_py3_generator : public t_mstch_generator {
  public:
    t_mstch_py3_generator(
        t_program* program,
        const std::map<std::string, std::string>& parsed_options,
        const std::string& /* option_string unused */)
        : t_mstch_generator(program, "py3", parsed_options) {

      this->out_dir_base_ = "gen-py3";
      auto include_prefix = this->get_option("include_prefix");
      if (include_prefix) {
        program->set_include_prefix(include_prefix.value());
      }
    }

    void generate_program() override;
    mstch::map extend_program(const t_program&) const override;
    mstch::map extend_type(const t_type&) const override;

  protected:
    void generate_init_files(const t_program&);
    void generate_structs(const t_program&);
    void generate_services(const t_program&);
    boost::filesystem::path package_to_path(std::string package);
    mstch::array get_return_types(const t_program&) const;
    mstch::array get_container_types(const t_program&) const;
    std::string get_cpp2_namespace(const t_program&) const;
    std::string flatten_type_name(const t_type&) const;

  private:
    void load_container_type(
      vector<t_type*>& container_types,
      std::set<string>& visited_names,
      t_type* type
    ) const;
};

mstch::map t_mstch_py3_generator::extend_program(
  const t_program& program
) const {
  auto cpp_namespace = this->get_cpp2_namespace(program);
  auto py3_namespace = program.get_namespace("py3");
  if (py3_namespace == "") {
    py3_namespace = "py3";
  }

  vector<string> ns;
  boost::algorithm::split(ns, cpp_namespace, boost::algorithm::is_any_of("."));
  auto cppNamespaces = this->dump_elems(ns);
  boost::algorithm::split(ns, py3_namespace, boost::algorithm::is_any_of("."));
  auto py3Namespaces = this->dump_elems(ns);
  ns.clear();
  for (auto included_program : program.get_includes()) {
    ns.push_back(
      included_program->get_namespace("py3") +
      "." +
      included_program->get_name());
  }
  auto includeNamespaces = this->dump_elems(ns);

  mstch::map result {
    {"returnTypes", this->get_return_types(program)},
    {"containerTypes", this->get_container_types(program)},
    {"cppNamespaces", cppNamespaces},
    {"py3Namespaces", py3Namespaces},
    {"includeNamespaces", includeNamespaces},
  };
  return result;
}

mstch::map t_mstch_py3_generator::extend_type(const t_type& type) const {
  auto program = type.get_program();
  if (!program) {
    program = this->get_program();
  }
  auto module_path =
    program->get_namespace("py3") +
    "." +
    program->get_name() +
    "_types";
  auto cpp_namespace = this->get_cpp2_namespace(*program);
  vector<string> ns;
  boost::algorithm::split(ns, cpp_namespace, boost::algorithm::is_any_of("."));
  auto cppNamespaces = this->dump_elems(ns);

  mstch::map result {
    {"module_path", module_path},
    {"flat_name", this->flatten_type_name(type)},
    {"cppNamespaces", cppNamespaces},
  };
  return result;
}

void t_mstch_py3_generator::generate_init_files(const t_program& program) {
  auto path = this->package_to_path(program.get_namespace("py3"));
  auto directory = boost::filesystem::path{};
  for (auto path_part : path) {
    directory /= path_part;
    this->write_output(directory / "__init__.py", "");
  }
}

void t_mstch_py3_generator::generate_structs(const t_program& program) {
  auto path = this->package_to_path(program.get_namespace("py3"));

  auto basename = program.get_name() + "_types";
  this->render_to_file(program, "Struct.pxd", path / (basename + ".pxd"));
  this->render_to_file(program, "Struct.pyx", path / (basename + ".pyx"));
}

void t_mstch_py3_generator::generate_services(const t_program& program) {
  auto path = this->package_to_path(program.get_namespace("py3"));

  auto name = this->get_program()->get_name();
  this->render_to_file(
    program, "Services.pxd", path / (name + "_services.pxd"));
  this->render_to_file(
    program, "CythonServices.pyx", path / (name + "_services.pyx"));

  auto basename = name + "_services_wrapper";
  this->render_to_file(
    program, "ServicesWrapper.h", path / (basename + ".h"));
  this->render_to_file(
    program, "ServicesWrapper.cpp", path / (basename + ".cpp"));
  this->render_to_file(
    program, "ServicesWrapper.pxd", path / (basename + ".pxd"));
}

boost::filesystem::path t_mstch_py3_generator::package_to_path(
  std::string package
) {
  boost::algorithm::replace_all(package, ".", "/");
  return boost::filesystem::path{package};
}

mstch::array t_mstch_py3_generator::get_return_types(
  const t_program& program
) const {
  mstch::array distinct_return_types;
  std::set<string> visited_names;

  for (const auto service : program.get_services()) {
    for (const auto function : service->get_functions()) {
      const auto returntype = function->get_returntype();
      string flat_name = this->flatten_type_name(*returntype);
      if (!visited_names.count(flat_name)) {
        distinct_return_types.push_back(this->dump(*returntype));
        visited_names.insert(flat_name);
      }
    }
  }
  return distinct_return_types;
}

mstch::array t_mstch_py3_generator::get_container_types(
  const t_program& program
) const {
  vector<t_type*> container_types;
  std::set<string> visited_names;

  for (const auto service : program.get_services()) {
    for (const auto function : service->get_functions()) {
      for (const auto field : function->get_arglist()->get_members()) {
        auto arg_type = field->get_type();
        this->load_container_type(
          container_types,
          visited_names,
          arg_type
        );
      }
      auto return_type = function->get_returntype();
      this->load_container_type(container_types, visited_names, return_type);
    }
  }
  for (const auto object :program.get_objects()) {
    for (const auto field : object->get_members()) {
      auto ref_type = field->get_type();
      this->load_container_type(container_types, visited_names, ref_type);
    }
  }
  return this->dump_elems(container_types);
}

void t_mstch_py3_generator::load_container_type(
  vector<t_type*>& container_types,
  std::set<string>& visited_names,
  t_type* type
) const {
  if (!type->is_container()) return;
  string flat_name = this->flatten_type_name(*type);
  if (visited_names.count(flat_name)) return;

  visited_names.insert(flat_name);
  container_types.push_back(type);
}

std::string t_mstch_py3_generator::flatten_type_name(const t_type& type) const {
    if (type.is_list()) {
      return "List__" + this->flatten_type_name(
        *dynamic_cast<const t_list&>(type).get_elem_type()
      );
  } else if (type.is_set()) {
    return "Set__" + this->flatten_type_name(
      *dynamic_cast<const t_set&>(type).get_elem_type()
    );
  } else if (type.is_map()) {
      return ("Map__" +
        this->flatten_type_name(
          *dynamic_cast<const t_map&>(type).get_key_type()
        ) + "_" +
        this->flatten_type_name(
          *dynamic_cast<const t_map&>(type).get_val_type()
        )
      );
  } else {
    return type.get_name();
  }
}

std::string t_mstch_py3_generator::get_cpp2_namespace(
  const t_program& program
) const {
  auto cpp_namespace = program.get_namespace("cpp2");
  if (cpp_namespace == "") {
    cpp_namespace = program.get_namespace("cpp");
    if (cpp_namespace == "") {
      cpp_namespace = "cpp2";
    }
    else {
      cpp_namespace = cpp_namespace + "cpp2";
    }
  }
  return cpp_namespace;
}

void t_mstch_py3_generator::generate_program() {
  mstch::config::escape = [](const std::string& s) { return s; };
  this->generate_init_files(*this->get_program());
  this->generate_structs(*this->get_program());
  this->generate_services(*this->get_program());
}

THRIFT_REGISTER_GENERATOR(
  mstch_py3,
  "Python 3",
  "    include_prefix:  Use full include paths in generated files.\n"
);
}
