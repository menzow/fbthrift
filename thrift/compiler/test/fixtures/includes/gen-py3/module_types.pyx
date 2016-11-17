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
cimport .includes_types



cdef class MyStruct:
    def __init__(
        self,
        Included MyIncludedField
    ):
        self.c_MyStruct = make_shared[cMyStruct]()
        deref(self.c_MyStruct).MyIncludedField = deref(MyIncludedField.c_Included)
        
    @staticmethod
    cdef create(shared_ptr[cMyStruct] c_MyStruct):
        inst = <MyStruct>MyStruct.__new__(MyStruct)
        inst.c_MyStruct = c_MyStruct
        return inst

    @property
    def MyIncludedField(self):
        cdef shared_ptr[cIncluded] item
        if self.__MyIncludedField is None:
            item = make_shared[cIncluded](deref(self.c_MyStruct).MyIncludedField)
            self.__MyIncludedField = Included.create(item)
        return self.__MyIncludedField
        





