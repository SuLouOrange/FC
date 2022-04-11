macro(SetupCoin3D)
# -------------------------------- Coin3D --------------------------------

    find_package(Coin3D NAMES coin)
    message(status "Coin3D 3d dir: ${Coin3D_DIR}")
    message(status "Coin3D lib dir: ${Coin_LIB_DIR}")
    if(NOT Coin3D_FOUND)
        message(FATAL_ERROR "=================\n"
                            "Coin3D not found.\n"
                            "=================\n")
    endif(NOT Coin3D_FOUND)

endmacro(SetupCoin3D)
