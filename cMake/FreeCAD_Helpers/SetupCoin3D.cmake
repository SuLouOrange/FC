macro(SetupCoin3D)
# -------------------------------- Coin3D --------------------------------

    find_package(Coin3D NAMES coin)
    message(status "Coin3D dir: ${Coin3D_DIR}")
    message(status "Coin lib dir: ${Coin_LIB_DIR}")
    set(Coin3D_LIBRARIES_DIR ${Coin_LIB_DIR})
    set(Coin4_LIB_DIR ${Coin_LIB_DIR})
    set(Coin3D_LIB_DIR ${Coin_LIB_DIR})
    set(Coin3_LIB_DIR ${Coin_LIB_DIR})
    if(NOT Coin3D_FOUND)
        message(FATAL_ERROR "=================\n"
                            "Coin3D not found.\n"
                            "=================\n")
    endif(NOT Coin3D_FOUND)

endmacro(SetupCoin3D)
