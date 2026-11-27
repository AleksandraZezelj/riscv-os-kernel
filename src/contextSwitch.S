.global _ZN3TCB13contextSwitchEPNS_7ContextES1_
.type _ZN3TCB13contextSwitchEPNS_7ContextES1_, @function

_ZN3TCB13contextSwitchEPNS_7ContextES1_: //EPN pointer na name(N) umesto 3CCB ponovo moze i S_ jer je to prvo definisano u imenu
    # a0 - &old->context
    # a1 - &running->context
    sd ra, 0 * 8 (a0)
    sd sp, 1 * 8 (a0)

    ld ra, 0 * 8 (a1)
    ld sp, 1 * 8 (a1)


    ret