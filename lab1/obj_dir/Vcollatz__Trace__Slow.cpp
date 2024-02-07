// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Tracing implementation internals
#include "verilated_vcd_c.h"
#include "Vcollatz__Syms.h"


//======================

void Vcollatz::trace(VerilatedVcdC* tfp, int, int) {
    tfp->spTrace()->addCallback(&Vcollatz::traceInit, &Vcollatz::traceFull, &Vcollatz::traceChg, this);
}
void Vcollatz::traceInit(VerilatedVcd* vcdp, void* userthis, uint32_t code) {
    // Callback from vcd->open()
    Vcollatz* t = (Vcollatz*)userthis;
    Vcollatz__Syms* __restrict vlSymsp = t->__VlSymsp;  // Setup global symbol table
    if (!Verilated::calcUnusedSigs()) {
        VL_FATAL_MT(__FILE__, __LINE__, __FILE__,
                        "Turning on wave traces requires Verilated::traceEverOn(true) call before time 0.");
    }
    vcdp->scopeEscape(' ');
    t->traceInitThis(vlSymsp, vcdp, code);
    vcdp->scopeEscape('.');
}
void Vcollatz::traceFull(VerilatedVcd* vcdp, void* userthis, uint32_t code) {
    // Callback from vcd->dump()
    Vcollatz* t = (Vcollatz*)userthis;
    Vcollatz__Syms* __restrict vlSymsp = t->__VlSymsp;  // Setup global symbol table
    t->traceFullThis(vlSymsp, vcdp, code);
}

//======================


void Vcollatz::traceInitThis(Vcollatz__Syms* __restrict vlSymsp, VerilatedVcd* vcdp, uint32_t code) {
    Vcollatz* __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    int c = code;
    if (0 && vcdp && c) {}  // Prevent unused
    vcdp->module(vlSymsp->name());  // Setup signal names
    // Body
    {
        vlTOPp->traceInitThis__1(vlSymsp, vcdp, code);
    }
}

void Vcollatz::traceFullThis(Vcollatz__Syms* __restrict vlSymsp, VerilatedVcd* vcdp, uint32_t code) {
    Vcollatz* __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    int c = code;
    if (0 && vcdp && c) {}  // Prevent unused
    // Body
    {
        vlTOPp->traceFullThis__1(vlSymsp, vcdp, code);
    }
    // Final
    vlTOPp->__Vm_traceActivity = 0U;
}

void Vcollatz::traceInitThis__1(Vcollatz__Syms* __restrict vlSymsp, VerilatedVcd* vcdp, uint32_t code) {
    Vcollatz* __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    int c = code;
    if (0 && vcdp && c) {}  // Prevent unused
    // Body
    {
        vcdp->declBit(c+1,"clk", false,-1);
        vcdp->declBit(c+9,"go", false,-1);
        vcdp->declBus(c+17,"n", false,-1, 31,0);
        vcdp->declBus(c+25,"dout", false,-1, 31,0);
        vcdp->declBit(c+33,"done", false,-1);
        vcdp->declBit(c+1,"collatz clk", false,-1);
        vcdp->declBit(c+9,"collatz go", false,-1);
        vcdp->declBus(c+17,"collatz n", false,-1, 31,0);
        vcdp->declBus(c+25,"collatz dout", false,-1, 31,0);
        vcdp->declBit(c+33,"collatz done", false,-1);
    }
}

void Vcollatz::traceFullThis__1(Vcollatz__Syms* __restrict vlSymsp, VerilatedVcd* vcdp, uint32_t code) {
    Vcollatz* __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    int c = code;
    if (0 && vcdp && c) {}  // Prevent unused
    // Body
    {
        vcdp->fullBit(c+1,(vlTOPp->clk));
        vcdp->fullBit(c+9,(vlTOPp->go));
        vcdp->fullBus(c+17,(vlTOPp->n),32);
        vcdp->fullBus(c+25,(vlTOPp->dout),32);
        vcdp->fullBit(c+33,(vlTOPp->done));
    }
}
