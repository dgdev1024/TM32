; Test trigonometric built-in functions
; Tests: sin(), cos(), tan(), asin(), acos(), atan(), atan2()

.section .data

; Test basic trigonometric functions
angle_0 .const = 0
angle_90 .const = 90

sin_0 .const = sin(angle_0)            ; Should be 0 (in fixed-point)
cos_0 .const = cos(angle_0)            ; Should be 65536 (1.0 in fixed-point)
tan_0 .const = tan(angle_0)            ; Should be 0

sin_90 .const = sin(angle_90)          ; Should be 65536 (1.0 in fixed-point)
cos_90 .const = cos(angle_90)          ; Should be ~0
tan_90 .const = tan(angle_90)          ; Should be very large

; Test inverse trigonometric functions
asin_0 .const = asin(0)                ; Should be 0
acos_1 .const = acos(1)                ; Should be 0
atan_0 .const = atan(0)                ; Should be 0
atan_1 .const = atan(1)                ; Should be ~45 degrees

; Test atan2 function (two parameters)
atan2_test .const = atan2(1, 1)        ; Should be ~45 degrees
atan2_quad2 .const = atan2(1, -1)      ; Should be ~135 degrees

; Test with small angles for precision
small_angle .const = 1
sin_small .const = sin(small_angle)
cos_small .const = cos(small_angle)

.section .text
start:
    nop
