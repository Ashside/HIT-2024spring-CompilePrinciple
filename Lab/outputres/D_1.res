Program (1)
  ExtDefList (1)
    ExtDef (1)
      Specifier (1)
        TYPE: int
      FunDec (1)
        ID: func_test
        LP
        RP
      CompSt (2)
        LC
        DefList (3)
          Def (3)
            Specifier (3)
              TYPE: int
            DecList (3)
              Dec (3)
                VarDec (3)
                  ID: _dec_
                ASSIGNOP
                Exp (3)
                  INT: 947
            SEMI
          DefList (4)
            Def (4)
              Specifier (4)
                TYPE: int
              DecList (4)
                Dec (4)
                  VarDec (4)
                    ID: _oct_
                  ASSIGNOP
                  Exp (4)
                    INT: 0705
              SEMI
            DefList (5)
              Def (5)
                Specifier (5)
                  TYPE: int
                DecList (5)
                  Dec (5)
                    VarDec (5)
                      ID: _dhex_
                    ASSIGNOP
                    Exp (5)
                      Exp (5)
                        INT: 0xFFaBc
                      MINUS
                      Exp (5)
                        ID: _oct_
                SEMI
              DefList (6)
                Def (6)
                  Specifier (6)
                    TYPE: int
                  DecList (6)
                    Dec (6)
                      VarDec (6)
                        ID: _result_
                      ASSIGNOP
                      Exp (6)
                        Exp (6)
                          MINUS
                          Exp (6)
                            ID: _dhex_
                        PLUS
                        Exp (6)
                          Exp (6)
                            ID: _oct_
                          STAR
                          Exp (6)
                            LP
                            Exp (6)
                              Exp (6)
                                ID: _dec_
                              MINUS
                              Exp (6)
                                INT: 0X23fD
                            RP
                  SEMI
        RC
