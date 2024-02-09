// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Tracing implementation internals
#include "verilated_vcd_c.h"
#include "Vrange__Syms.h"


//======================

void Vrange::traceChg(VerilatedVcd* vcdp, void* userthis, uint32_t code) {
    // Callback from vcd->dump()
    Vrange* t = (Vrange*)userthis;
    Vrange__Syms* __restrict vlSymsp = t->__VlSymsp;  // Setup global symbol table
    if (vlSymsp->getClearActivity()) {
        t->traceChgThis(vlSymsp, vcdp, code);
    }
}

//======================


void Vrange::traceChgThis(Vrange__Syms* __restrict vlSymsp, VerilatedVcd* vcdp, uint32_t code) {
    Vrange* __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    int c = code;
    if (0 && vcdp && c) {}  // Prevent unused
    // Body
    {
        if (VL_UNLIKELY((1U & (vlTOPp->__Vm_traceActivity 
                               | (vlTOPp->__Vm_traceActivity 
                                  >> 1U))))) {
            vlTOPp->traceChgThis__2(vlSymsp, vcdp, code);
        }
        if (VL_UNLIKELY((1U & (vlTOPp->__Vm_traceActivity 
                               | (vlTOPp->__Vm_traceActivity 
                                  >> 2U))))) {
            vlTOPp->traceChgThis__3(vlSymsp, vcdp, code);
        }
        if (VL_UNLIKELY((2U & vlTOPp->__Vm_traceActivity))) {
            vlTOPp->traceChgThis__4(vlSymsp, vcdp, code);
        }
        vlTOPp->traceChgThis__5(vlSymsp, vcdp, code);
    }
    // Final
    vlTOPp->__Vm_traceActivity = 0U;
}

void Vrange::traceChgThis__2(Vrange__Syms* __restrict vlSymsp, VerilatedVcd* vcdp, uint32_t code) {
    Vrange* __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    int c = code;
    if (0 && vcdp && c) {}  // Prevent unused
    // Body
    {
        vcdp->chgBit(c+1,(vlTOPp->range__DOT__running));
    }
}

void Vrange::traceChgThis__3(Vrange__Syms* __restrict vlSymsp, VerilatedVcd* vcdp, uint32_t code) {
    Vrange* __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    int c = code;
    if (0 && vcdp && c) {}  // Prevent unused
    // Body
    {
        vcdp->chgBus(c+9,(vlTOPp->range__DOT__addr),4);
    }
}

void Vrange::traceChgThis__4(Vrange__Syms* __restrict vlSymsp, VerilatedVcd* vcdp, uint32_t code) {
    Vrange* __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    int c = code;
    if (0 && vcdp && c) {}  // Prevent unused
    // Body
    {
        vcdp->chgBit(c+17,(vlTOPp->range__DOT__cgo));
        vcdp->chgBit(c+25,((1U == vlTOPp->range__DOT__dout_)));
        vcdp->chgBus(c+33,(vlTOPp->range__DOT__n),32);
        vcdp->chgBus(c+41,(vlTOPp->range__DOT__dout_),32);
        vcdp->chgBus(c+49,(vlTOPp->range__DOT__num),4);
        vcdp->chgBit(c+57,(vlTOPp->range__DOT__we));
        vcdp->chgBus(c+65,(vlTOPp->range__DOT__din),16);
        vcdp->chgBus(c+73,(vlTOPp->range__DOT__mem[0]),16);
        vcdp->chgBus(c+74,(vlTOPp->range__DOT__mem[1]),16);
        vcdp->chgBus(c+75,(vlTOPp->range__DOT__mem[2]),16);
        vcdp->chgBus(c+76,(vlTOPp->range__DOT__mem[3]),16);
        vcdp->chgBus(c+77,(vlTOPp->range__DOT__mem[4]),16);
        vcdp->chgBus(c+78,(vlTOPp->range__DOT__mem[5]),16);
        vcdp->chgBus(c+79,(vlTOPp->range__DOT__mem[6]),16);
        vcdp->chgBus(c+80,(vlTOPp->range__DOT__mem[7]),16);
        vcdp->chgBus(c+81,(vlTOPp->range__DOT__mem[8]),16);
        vcdp->chgBus(c+82,(vlTOPp->range__DOT__mem[9]),16);
        vcdp->chgBus(c+83,(vlTOPp->range__DOT__mem[10]),16);
        vcdp->chgBus(c+84,(vlTOPp->range__DOT__mem[11]),16);
        vcdp->chgBus(c+85,(vlTOPp->range__DOT__mem[12]),16);
        vcdp->chgBus(c+86,(vlTOPp->range__DOT__mem[13]),16);
        vcdp->chgBus(c+87,(vlTOPp->range__DOT__mem[14]),16);
        vcdp->chgBus(c+88,(vlTOPp->range__DOT__mem[15]),16);
    }
}

void Vrange::traceChgThis__5(Vrange__Syms* __restrict vlSymsp, VerilatedVcd* vcdp, uint32_t code) {
    Vrange* __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    int c = code;
    if (0 && vcdp && c) {}  // Prevent unused
    // Body
    {
        vcdp->chgBit(c+201,(vlTOPp->clk));
        vcdp->chgBit(c+209,(vlTOPp->go));
        vcdp->chgBus(c+217,(vlTOPp->start),32);
        vcdp->chgBit(c+225,(vlTOPp->done));
        vcdp->chgBus(c+233,(vlTOPp->count),16);
    }
}
