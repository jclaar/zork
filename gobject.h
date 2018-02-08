// Generated at 02-07-18 17:59:51
make_gobj(std::string(), { "IT", "THAT", "THIS", "HIM" }, {  }, "random object", { ovison, ndescbit } ),
make_gobj(std::string(), { "GBROC", "BROCH", "MAIL" }, { "FREE" }, "free brochure", { ovison } , obj_funcs::brochure),
make_gobj(std::string(), { "#####", "ME", "CRETI", "MYSEL", "SELF" }, {  }, "cretin", { ovison, villain } , obj_funcs::cretin, { }, { OP(ksl_oglobal, 0), OP(ksl_oactor, oa_player) }),
make_gobj(std::string(), { "WISH", "BLESS" }, {  }, "wish", { ovison, ndescbit } ),
make_gobj(std::string(), { "EVERY", "ALL" }, {  }, "everything", { ovison, takebit, ndescbit, no_check_bit, bunchbit } , obj_funcs::valuables_c),
make_gobj(std::string(), { "POSSE" }, {  }, "possessions", { ovison, takebit, ndescbit, no_check_bit, bunchbit } , obj_funcs::valuables_c),
make_gobj(std::string(), { "VALUA", "TREAS" }, {  }, "valuables", { ovison, takebit, ndescbit, no_check_bit, bunchbit } , obj_funcs::valuables_c),
make_gobj(std::string(), { "SAILO" }, {  }, "sailor", { ovison, ndescbit } ),
make_gobj(std::string(), { "TEETH" }, {  }, "set of teeth", { ovison, ndescbit } ),
make_gobj(std::string(), { "WALL", "WALLS" }, {  }, "wall", { ovison } , obj_funcs::wall_function),
make_gobj(std::string(), { "GWALL", "WALL" }, { "GRANI" }, "granite wall", { ovison } , obj_funcs::granite),
make_gobj(std::string(), { "GROUN", "EARTH", "SAND" }, {  }, "ground", { ovison, digbit } , obj_funcs::ground_function),
make_gobj(std::string(), { "GRUE" }, {  }, "lurking grue", { ovison } , obj_funcs::grue_function),
make_gobj(std::string(), { "HANDS", "HAND" }, { "BARE" }, "pair of hands", { ovison, ndescbit, toolbit } ),
make_gobj(std::string(), { "LUNGS", "AIR" }, {  }, "breath", { ovison, ndescbit, toolbit } ),
make_gobj(std::string(), { "AVIAT", "FLYER" }, {  }, "flyer", { ovison, ndescbit } ),
make_gobj(std::string(), { "EXCEP", "BUT" }, {  }, "moby lossage", { ovison, ndescbit } ),
make_gobj("WELLBIT", { "WELL" }, { "MAGIC" }, "well", { ovison, ndescbit } , obj_funcs::well_function),
make_gobj("ROPEBIT", { "SROPE", "ROPE", "PIECE" }, {  }, "piece of rope", { ovison, climbbit, no_check_bit } , obj_funcs::slide_rope),
make_gobj("SLIDEBIT", { "SLIDE", "CHUTE" }, {  }, "chute", { ovison} , obj_funcs::slide_function),
make_gobj("CPWALL", { "CPEWL", "WALL" }, { "EAST", "EASTE" }, "eastern wall", { ovison} , obj_funcs::cpwall_object),
make_gobj("CPWALL", { "CPWWL", "WALL" }, { "WEST", "WESTE" }, "western wall", { ovison} , obj_funcs::cpwall_object),
make_gobj("CPWALL", { "CPSWL", "WALL" }, { "SOUTH" }, "southern wall", { ovison} , obj_funcs::cpwall_object),
make_gobj("CPWALL", { "CPNWL", "WALL" }, { "NORTH" }, "northern wall", { ovison} , obj_funcs::cpwall_object),
make_gobj("CPLADDER", { "CPLAD", "LADDE" }, {  }, "ladder", { ovison} , obj_funcs::cpladder_object),
make_gobj("BIRDBIT", { "BIRD", "SONGB" }, { "SONG" }, "bird", { ovison, ndescbit } , obj_funcs::bird_object),
make_gobj("HOUSEBIT", { "HOUSE" }, { "WHITE" }, "white house", { ovison, ndescbit } , obj_funcs::house_function),
make_gobj("TREEBIT", { "TREE" }, {  }, "tree", { ovison, ndescbit } ),
make_gobj("GUARDBIT", { "GUARD" }, {  }, "Guardian of Zork", { ovison, vicbit, villain } , obj_funcs::guardians),
make_gobj("ROSEBIT", { "ROSE", "COMPA" }, {  }, "compass rose", { ovison } ),
make_gobj("MASTERBIT", { "MASTE", "KEEPE", "DUNGE" }, { "DUNGE" }, "dungeon master", { ovison, vicbit, actorbit } , obj_funcs::master_function, { }, { OP(ksl_odesc1, "The dungeon master is quietly leaning on his staff here."), OP(ksl_oactor, oa_master), OP(ksl_oglobal, 0) }),
make_gobj("MIRRORBIT", { "MIRRO", "STRUC" }, {  }, "mirror", { ovison } , obj_funcs::mirror_function),
make_gobj("PANELBIT", { "PANEL" }, {  }, "panel", { ovison } , obj_funcs::panel_function),
make_gobj("CHANBIT", { "CHANN" }, {  }, "stone channel", { ovison } ),
make_gobj("WALL-ESWBIT", { "WEAST", "WALL" }, { "EAST", "EASTE" }, "eastern wall", { ovison, ndescbit } , obj_funcs::scolwall),
make_gobj("WALL-ESWBIT", { "WSOUT", "WALL" }, { "SOUTH" }, "southern wall", { ovison, ndescbit } , obj_funcs::scolwall),
make_gobj("WALL-ESWBIT", { "WWEST", "WALL" }, { "WEST", "WESTE" }, "western wall", { ovison, ndescbit } , obj_funcs::scolwall),
make_gobj("WALL-NBIT", { "WNORT", "WALL" }, { "NORTH" }, "northern wall", { ovison, ndescbit } , obj_funcs::scolwall),
make_gobj("RGWATER", { "GWATE", "WATER", "QUANT", "LIQUI", "H2O" }, {  }, "water", { drinkbit, ovison } , obj_funcs::water_function),
make_gobj("DWINDOW", { "WINDO" }, {  }, "window", { ovison} , nullptr),
