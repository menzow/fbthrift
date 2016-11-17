#
# Autogenerated by Thrift
#
# DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
#  @generated
#

from libcpp.memory cimport shared_ptr, make_shared, unique_ptr, make_unique
from libcpp.string cimport string
from libcpp cimport bool as cbool
from cpython cimport bool as pbool
from libc.stdint cimport int8_t, int16_t, int32_t, int64_t
from cython.operator cimport dereference as deref
from thrift.lib.py3.thrift_server cimport TException

from collections.abc import Sequence, Set, Mapping
from enum import Enum
cimport py3.module_types



cdef class Empty:
    def __init__(
        self
    ):
        self.c_Empty = make_shared[cEmpty]()
        pass

    @staticmethod
    cdef create(shared_ptr[cEmpty] c_Empty):
        inst = <Empty>Empty.__new__(Empty)
        inst.c_Empty = c_Empty
        return inst





