// Autogenerated by Thrift Compiler (facebook)
// DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
// @generated

package module

import (
	"bytes"
	"fmt"
	"git.apache.org/thrift.git/lib/go/thrift"
)

// (needed to ensure safety because of naive import list construction.)
var _ = thrift.ZERO
var _ = fmt.Printf
var _ = bytes.Equal

type MyServiceEmpty interface {}

type MyServiceEmptyClient struct {
  Transport thrift.TTransport
  ProtocolFactory thrift.TProtocolFactory
  InputProtocol thrift.TProtocol
  OutputProtocol thrift.TProtocol
  SeqId int32
}

func NewMyServiceEmptyClientFactory(t thrift.TTransport, f thrift.TProtocolFactory) *MyServiceEmptyClient {
  return &MyServiceEmptyClient{Transport: t,
    ProtocolFactory: f,
    InputProtocol: f.GetProtocol(t),
    OutputProtocol: f.GetProtocol(t),
    SeqId: 0,
  }
}

func NewMyServiceEmptyClientProtocol(t thrift.TTransport, iprot thrift.TProtocol, oprot thrift.TProtocol) *MyServiceEmptyClient {
  return &MyServiceEmptyClient{Transport: t,
    ProtocolFactory: nil,
    InputProtocol: iprot,
    OutputProtocol: oprot,
    SeqId: 0,
  }
}


type MyServiceEmptyProcessor struct {
  processorMap map[string]thrift.TProcessorFunction
  handler MyServiceEmpty
}

func (p *MyServiceEmptyProcessor) AddToProcessorMap(key string, processor thrift.TProcessorFunction) {
  p.processorMap[key] = processor
}

func (p *MyServiceEmptyProcessor) GetProcessorFunction(key string) (processor thrift.TProcessorFunction, ok bool) {
  processor, ok = p.processorMap[key]
  return processor, ok
}

func (p *MyServiceEmptyProcessor) ProcessorMap() map[string]thrift.TProcessorFunction {
  return p.processorMap
}

func NewMyServiceEmptyProcessor(handler MyServiceEmpty) *MyServiceEmptyProcessor {

  self36 := &MyServiceEmptyProcessor{handler:handler, processorMap:make(map[string]thrift.TProcessorFunction)}
return self36
}

func (p *MyServiceEmptyProcessor) Process(iprot, oprot thrift.TProtocol) (success bool, err thrift.TException) {
  name, _, seqId, err := iprot.ReadMessageBegin()
  if err != nil { return false, err }
  if processor, ok := p.GetProcessorFunction(name); ok {
    return processor.Process(seqId, iprot, oprot)
  }
  iprot.Skip(thrift.STRUCT)
  iprot.ReadMessageEnd()
  x37 := thrift.NewTApplicationException(thrift.UNKNOWN_METHOD, "Unknown function " + name)
  oprot.WriteMessageBegin(name, thrift.EXCEPTION, seqId)
  x37.Write(oprot)
  oprot.WriteMessageEnd()
  oprot.Flush()
  return false, x37

}


// HELPER FUNCTIONS AND STRUCTURES


