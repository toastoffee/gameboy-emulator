TileData::
    db $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
    db $ff, $ff, $ff, $ff, $ff, $ff, $ff, $ff, $ff, $ff, $ff, $ff, $ff, $ff, $ff, $ff
    db $ff, $00, $ff, $00, $ff, $00, $ff, $00, $ff, $00, $ff, $00, $ff, $00, $ff, $00

    ; 3: one corner of eye
    dw `11111133
    dw `11113333
    dw `11133300
    dw `11330000
    dw `13300000
    dw `13300000
    dw `33000000
    dw `33000000

    ; 4: one corner of the nose
    dw `00000001
    dw `00000011
    dw `00000111
    dw `00001111
    dw `00011111
    dw `00111111
    dw `01111111
    dw `11111111

    ; 5: eye lash top
    dw `00002200
    dw `00233200
    dw `02332000
    dw `03320000
    dw `23320000
    dw `33200000
    dw `33200000
    dw `33200000

    ; 6: eye lash bottom
    dw `23300000
    dw `03300000
    dw `02320000
    dw `00230000
    dw `00022000
    dw `00000000
    dw `00000000
    dw `00000000

    ; filler
    db $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
    db $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00

    ; 9 mole
    dw `00000000
    dw `00000000
    dw `00003300
    dw `00033330
    dw `00033330
    dw `00003300
    dw `00000000
    dw `00000000

    ; 10 mole-cover
    dw `11111111
    dw `11111111
    dw `11111111
    dw `11111111
    dw `11111111
    dw `11111111
    dw `11111111
    dw `11111111

    ; filler
    db $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00

    ; 12 mouth
    db $ff, $ff, $ff, $ff, $ff, $ff, $ff, $ff, $ff, $ff, $ff, $ff, $ff, $ff, $ff, $ff
    db $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00

    ; 14 (should not be used for mouth on 8x16 object with tile index 13)
    db $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00

    ; 15 tongue
    db 00, $ff, $00, $ff, $00, $ff, $00, $ff, $00, $ff, $00, $ff, $00, $ff, $00, $ff

    ; 16 hair
    dw `30023002
    dw `32323232
    dw `32323232
    dw `32323232
    dw `32323232
    dw `32323232
    dw `32323232
    dw `32323232

.end:


TileDataA0::
    ; $a0: top right quadrant corner of eye
    dw `33111111
    dw `33331111
    dw `00333111
    dw `22003311
    dw `22200331
    dw `22220331
    dw `22222033
    dw `22222033

    ; $a1: bottom left quadrant of eye
    dw `33000000
    dw `33000000
    dw `13300000
    dw `13300000
    dw `11330000
    dw `11133300
    dw `11113333
    dw `11111133

    ; $a2: bottom right quadrant of eye
    dw `22222033
    dw `22222033
    dw `22220331
    dw `22200331
    dw `22003311
    dw `00333111
    dw `33331111
    dw `33111111

    ; $a3: eye filling object
    dw `33333333
    dw `33333333
    dw `33333300
    dw `33330022
    dw `33300222
    dw `33302222
    dw `33022222
    dw `33022222    
.end:
