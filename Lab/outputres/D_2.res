Program (1)
  ExtDefList (1)
    ExtDef (1)
      Specifier (1)
        TYPE: int
      FunDec (1)
        ID: float_test
        LP
        RP
      CompSt (2)
        LC
        DefList (3)
          Def (3)
            Specifier (3)
              TYPE: float
            DecList (3)
              Dec (3)
                VarDec (3)
                  ID: X_1
                ASSIGNOP
                Exp (3)
                  FLOAT: 0.000000
            SEMI
          DefList (4)
            Def (4)
              Specifier (4)
                TYPE: float
              DecList (4)
                Dec (4)
                  VarDec (4)
                    ID: X_2
                  ASSIGNOP
                  Exp (4)
                    FLOAT: 3452000.000000
              SEMI
            DefList (5)
              Def (5)
                Specifier (5)
                  TYPE: float
                DecList (5)
                  Dec (5)
                    VarDec (5)
                      ID: X_3
                    ASSIGNOP
                    Exp (5)
                      FLOAT: 2760.000000
                SEMI
              DefList (6)
                Def (6)
                  Specifier (6)
                    TYPE: float
                  DecList (6)
                    Dec (6)
                      VarDec (6)
                        ID: X_4
                      ASSIGNOP
                      Exp (6)
                        FLOAT: 0.009940
                  SEMI
                DefList (7)
                  Def (7)
                    Specifier (7)
                      TYPE: float
                    DecList (7)
                      Dec (7)
                        VarDec (7)
                          ID: X_5
                        ASSIGNOP
                        Exp (7)
                          FLOAT: 110.000000
                    SEMI
                  DefList (8)
                    Def (8)
                      Specifier (8)
                        TYPE: float
                      DecList (8)
                        Dec (8)
                          VarDec (8)
                            ID: X_6
                          ASSIGNOP
                          Exp (8)
                            FLOAT: -0.000400
                      SEMI
                    DefList (9)
                      Def (9)
                        Specifier (9)
                          TYPE: float
                        DecList (9)
                          Dec (9)
                            VarDec (9)
                              ID: result
                            ASSIGNOP
                            Exp (9)
                              Exp (9)
                                LP
                                Exp (9)
                                  Exp (9)
                                    FLOAT: 1.500000
                                  PLUS
                                  Exp (9)
                                    ID: X_3
                                RP
                              PLUS
                              Exp (9)
                                ID: X_6
                        SEMI
        RC
