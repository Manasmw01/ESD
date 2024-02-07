// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vrange.h for the primary calling header

#include "Vrange.h"
#include "Vrange__Syms.h"

//==========

VL_CTOR_IMP(Vrange) {
    Vrange__Syms* __restrict vlSymsp = __VlSymsp = new Vrange__Syms(this, name());
    Vrange* __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Reset internal values
    
    // Reset structure values
    _ctor_var_reset();
}

void Vrange::__Vconfigure(Vrange__Syms* vlSymsp, bool first) {
    if (0 && first) {}  // Prevent unused
    this->__VlSymsp = vlSymsp;
}

Vrange::~Vrange() {
    delete __VlSymsp; __VlSymsp=NULL;
}

void Vrange::eval() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vrange::eval\n"); );
    Vrange__Syms* __restrict vlSymsp = this->__VlSymsp;  // Setup global symbol table
    Vrange* __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
#ifdef VL_DEBUG
    // Debug assertions
    _eval_debug_assertions();
#endif  // VL_DEBUG
    // Initialize
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) _eval_initial_loop(vlSymsp);
    // Evaluate till stable
    int __VclockLoop = 0;
    QData __Vchange = 1;
    do {
        VL_DEBUG_IF(VL_DBG_MSGF("+ Clock loop\n"););
        vlSymsp->__Vm_activity = true;
        _eval(vlSymsp);
        if (VL_UNLIKELY(++__VclockLoop > 100)) {
            // About to fail, so enable debug to see what's not settling.
            // Note you must run make with OPT=-DVL_DEBUG for debug prints.
            int __Vsaved_debug = Verilated::debug();
            Verilated::debug(1);
            __Vchange = _change_request(vlSymsp);
            Verilated::debug(__Vsaved_debug);
            VL_FATAL_MT("range.sv", 1, "",
                "Verilated model didn't converge\n"
                "- See DIDNOTCONVERGE in the Verilator manual");
        } else {
            __Vchange = _change_request(vlSymsp);
        }
    } while (VL_UNLIKELY(__Vchange));
}

void Vrange::_eval_initial_loop(Vrange__Syms* __restrict vlSymsp) {
    vlSymsp->__Vm_didInit = true;
    _eval_initial(vlSymsp);
    vlSymsp->__Vm_activity = true;
    // Evaluate till stable
    int __VclockLoop = 0;
    QData __Vchange = 1;
    do {
        _eval_settle(vlSymsp);
        _eval(vlSymsp);
        if (VL_UNLIKELY(++__VclockLoop > 100)) {
            // About to fail, so enable debug to see what's not settling.
            // Note you must run make with OPT=-DVL_DEBUG for debug prints.
            int __Vsaved_debug = Verilated::debug();
            Verilated::debug(1);
            __Vchange = _change_request(vlSymsp);
            Verilated::debug(__Vsaved_debug);
            VL_FATAL_MT("range.sv", 1, "",
                "Verilated model didn't DC converge\n"
                "- See DIDNOTCONVERGE in the Verilator manual");
        } else {
            __Vchange = _change_request(vlSymsp);
        }
    } while (VL_UNLIKELY(__Vchange));
}

VL_INLINE_OPT void Vrange::_sequent__TOP__1(Vrange__Syms* __restrict vlSymsp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrange::_sequent__TOP__1\n"); );
    Vrange* __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Variables
    CData/*0:0*/ __Vdly__range__DOT__running;
    CData/*3:0*/ __Vdly__range__DOT__num;
    CData/*0:0*/ __Vdly__range__DOT__cgo;
    CData/*3:0*/ __Vdlyvdim0__range__DOT__mem__v0;
    CData/*0:0*/ __Vdlyvset__range__DOT__mem__v0;
    SData/*15:0*/ __Vdly__range__DOT__din;
    SData/*15:0*/ __Vdlyvval__range__DOT__mem__v0;
    IData/*31:0*/ __Vdly__range__DOT__n;
    IData/*31:0*/ __Vdly__range__DOT__dout_;
    // Body
    __Vdly__range__DOT__dout_ = vlTOPp->range__DOT__dout_;
    __Vdlyvset__range__DOT__mem__v0 = 0U;
    __Vdly__range__DOT__din = vlTOPp->range__DOT__din;
    __Vdly__range__DOT__cgo = vlTOPp->range__DOT__cgo;
    __Vdly__range__DOT__n = vlTOPp->range__DOT__n;
    __Vdly__range__DOT__running = vlTOPp->range__DOT__running;
    __Vdly__range__DOT__num = vlTOPp->range__DOT__num;
    if (vlTOPp->range__DOT__cgo) {
        __Vdly__range__DOT__dout_ = vlTOPp->range__DOT__n;
    } else {
        if ((1U < vlTOPp->range__DOT__dout_)) {
            __Vdly__range__DOT__dout_ = ((1U == (1U 
                                                 & vlTOPp->range__DOT__dout_))
                                          ? ((IData)(1U) 
                                             + ((IData)(3U) 
                                                * vlTOPp->range__DOT__dout_))
                                          : (vlTOPp->range__DOT__dout_ 
                                             >> 1U));
        }
    }
    vlTOPp->count = vlTOPp->range__DOT__mem[vlTOPp->range__DOT__addr];
    if (vlTOPp->range__DOT__we) {
        __Vdlyvval__range__DOT__mem__v0 = vlTOPp->range__DOT__din;
        __Vdlyvset__range__DOT__mem__v0 = 1U;
        __Vdlyvdim0__range__DOT__mem__v0 = vlTOPp->range__DOT__addr;
    }
    if (vlTOPp->go) {
        __Vdly__range__DOT__running = 1U;
        __Vdly__range__DOT__n = vlTOPp->start;
        __Vdly__range__DOT__num = 0U;
        __Vdly__range__DOT__cgo = 1U;
        __Vdly__range__DOT__din = 1U;
    }
    if (vlTOPp->done) {
        vlTOPp->done = 0U;
    }
    if (vlTOPp->range__DOT__cgo) {
        __Vdly__range__DOT__cgo = 0U;
        __Vdly__range__DOT__din = 1U;
    }
    if (vlTOPp->range__DOT__we) {
        __Vdly__range__DOT__n = ((IData)(1U) + vlTOPp->range__DOT__n);
        vlTOPp->range__DOT__we = 0U;
        __Vdly__range__DOT__cgo = (0xfU != (IData)(vlTOPp->range__DOT__num));
        if ((0xfU != (IData)(vlTOPp->range__DOT__num))) {
            __Vdly__range__DOT__num = (0xfU & ((IData)(1U) 
                                               + (IData)(vlTOPp->range__DOT__num)));
        }
        if ((0xfU == (IData)(vlTOPp->range__DOT__num))) {
            __Vdly__range__DOT__running = 0U;
            vlTOPp->done = 1U;
        }
    }
    if (vlTOPp->range__DOT__running) {
        if (((1U != vlTOPp->range__DOT__dout_) & (~ (IData)(vlTOPp->range__DOT__cgo)))) {
            __Vdly__range__DOT__din = (0xffffU & ((IData)(1U) 
                                                  + (IData)(vlTOPp->range__DOT__din)));
        }
        if ((2U == vlTOPp->range__DOT__dout_)) {
            vlTOPp->range__DOT__we = 1U;
        }
    }
    if (__Vdlyvset__range__DOT__mem__v0) {
        vlTOPp->range__DOT__mem[__Vdlyvdim0__range__DOT__mem__v0] 
            = __Vdlyvval__range__DOT__mem__v0;
    }
    vlTOPp->range__DOT__cgo = __Vdly__range__DOT__cgo;
    vlTOPp->range__DOT__n = __Vdly__range__DOT__n;
    vlTOPp->range__DOT__dout_ = __Vdly__range__DOT__dout_;
    vlTOPp->range__DOT__din = __Vdly__range__DOT__din;
    vlTOPp->range__DOT__running = __Vdly__range__DOT__running;
    vlTOPp->range__DOT__num = __Vdly__range__DOT__num;
}

void Vrange::_initial__TOP__2(Vrange__Syms* __restrict vlSymsp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrange::_initial__TOP__2\n"); );
    Vrange* __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Body
    vlTOPp->range__DOT__running = 0U;
}

VL_INLINE_OPT void Vrange::_settle__TOP__3(Vrange__Syms* __restrict vlSymsp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrange::_settle__TOP__3\n"); );
    Vrange* __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Body
    vlTOPp->range__DOT__addr = (0xfU & ((IData)(vlTOPp->range__DOT__we)
                                         ? (IData)(vlTOPp->range__DOT__num)
                                         : vlTOPp->start));
}

void Vrange::_eval(Vrange__Syms* __restrict vlSymsp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrange::_eval\n"); );
    Vrange* __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Body
    if (((IData)(vlTOPp->clk) & (~ (IData)(vlTOPp->__Vclklast__TOP__clk)))) {
        vlTOPp->_sequent__TOP__1(vlSymsp);
        vlTOPp->__Vm_traceActivity = (2U | vlTOPp->__Vm_traceActivity);
    }
    vlTOPp->_settle__TOP__3(vlSymsp);
    vlTOPp->__Vm_traceActivity = (4U | vlTOPp->__Vm_traceActivity);
    // Final
    vlTOPp->__Vclklast__TOP__clk = vlTOPp->clk;
}

void Vrange::_eval_initial(Vrange__Syms* __restrict vlSymsp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrange::_eval_initial\n"); );
    Vrange* __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Body
    vlTOPp->__Vclklast__TOP__clk = vlTOPp->clk;
    vlTOPp->_initial__TOP__2(vlSymsp);
    vlTOPp->__Vm_traceActivity = (1U | vlTOPp->__Vm_traceActivity);
}

void Vrange::final() {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrange::final\n"); );
    // Variables
    Vrange__Syms* __restrict vlSymsp = this->__VlSymsp;
    Vrange* __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
}

void Vrange::_eval_settle(Vrange__Syms* __restrict vlSymsp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrange::_eval_settle\n"); );
    Vrange* __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Body
    vlTOPp->_settle__TOP__3(vlSymsp);
    vlTOPp->__Vm_traceActivity = (1U | vlTOPp->__Vm_traceActivity);
}

VL_INLINE_OPT QData Vrange::_change_request(Vrange__Syms* __restrict vlSymsp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrange::_change_request\n"); );
    Vrange* __restrict vlTOPp VL_ATTR_UNUSED = vlSymsp->TOPp;
    // Body
    // Change detection
    QData __req = false;  // Logically a bool
    return __req;
}

#ifdef VL_DEBUG
void Vrange::_eval_debug_assertions() {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrange::_eval_debug_assertions\n"); );
    // Body
    if (VL_UNLIKELY((clk & 0xfeU))) {
        Verilated::overWidthError("clk");}
    if (VL_UNLIKELY((go & 0xfeU))) {
        Verilated::overWidthError("go");}
}
#endif  // VL_DEBUG

void Vrange::_ctor_var_reset() {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vrange::_ctor_var_reset\n"); );
    // Body
    clk = VL_RAND_RESET_I(1);
    go = VL_RAND_RESET_I(1);
    start = VL_RAND_RESET_I(32);
    done = VL_RAND_RESET_I(1);
    count = VL_RAND_RESET_I(16);
    range__DOT__cgo = VL_RAND_RESET_I(1);
    range__DOT__n = VL_RAND_RESET_I(32);
    range__DOT__dout_ = VL_RAND_RESET_I(32);
    range__DOT__num = VL_RAND_RESET_I(4);
    range__DOT__running = VL_RAND_RESET_I(1);
    range__DOT__we = VL_RAND_RESET_I(1);
    range__DOT__din = VL_RAND_RESET_I(16);
    { int __Vi0=0; for (; __Vi0<16; ++__Vi0) {
            range__DOT__mem[__Vi0] = VL_RAND_RESET_I(16);
    }}
    range__DOT__addr = VL_RAND_RESET_I(4);
    __Vm_traceActivity = 0;
}
