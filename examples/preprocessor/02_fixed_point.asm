;
; @file     examples/preprocessor/02_fixed_point.asm
; @brief    Contains code which demonstrates the TM32 Assembler Tool's
;           Preprocessor Component's ability to evaluate expressions involving
;           fixed-point numbers, including storing them in variables and constants.
;

; Run this file with:
;   `tm32asm --preprocess-only examples/preprocessor/02_fixed_point.asm`

; ============================================================================
; FIXED-POINT VARIABLES AND CONSTANTS
; ============================================================================

; Basic fixed-point constants
.const PI = 3.14159
.const E = 2.71828
.const HALF = 0.5
.const QUARTER = 0.25
.const GOLDEN_RATIO = 1.618033988

; Mathematical constants
.const SQRT_2 = 1.41421356
.const SQRT_3 = 1.73205080
.const PI_OVER_2 = 1.5707963
.const TWO_PI = 6.28318530

; Common fractions as fixed-point constants
.const ONE_THIRD = 0.33333333
.const TWO_THIRDS = 0.66666667
.const THREE_QUARTERS = 0.75
.const ONE_EIGHTH = 0.125

; Fixed-point variables with initial values
.let fp_var_a = 2.5
.let fp_var_b = 1.75
.let fp_var_c = 0.0
.let temperature = 98.6
.let voltage = 3.3

; Variables initialized with expressions
.let area = PI * 2.0 * 2.0
.let circumference = TWO_PI * 5.0
.let diagonal = SQRT_2 * 10.0

; Compound assignments (sum_value operations)
.let sum_value = 0.0
.let sum_value += 1.5
.let sum_value *= 2.0
.let sum_value -= 0.25

; ============================================================================
; BASIC FIXED-POINT ARITHMETIC WITH LITERALS
; ============================================================================

; Simple addition
.let add_result_1 = 0.2 + 0.4
.let add_result_2 = 1.5 + 2.5
.let add_result_3 = 3.25 + 1.75
.let add_result_4 = 0.1 + 0.9

; Simple subtraction
.let sub_result_1 = 1.5 - 0.5
.let sub_result_2 = 5.0 - 2.3
.let sub_result_3 = 10.75 - 3.25
.let sub_result_4 = 1.0 - 0.1

; Simple multiplication
.let mult_result_1 = 2.5 * 4.0
.let mult_result_2 = 1.5 * 2.0
.let mult_result_3 = 0.5 * 8.0
.let mult_result_4 = 3.14 * 2.0

; Simple division
.let div_result_1 = 10.0 / 2.5
.let div_result_2 = 7.5 / 1.5
.let div_result_3 = 12.0 / 3.0
.let div_result_4 = 1.0 / 4.0

; ============================================================================
; MIXED INTEGER AND FIXED-POINT OPERATIONS
; ============================================================================

; Integer + Fixed-point
.let mixed_add_1 = 5 + 2.5
.let mixed_add_2 = 10 + 0.75
.let mixed_add_3 = 100 + 3.14159
.let mixed_add_4 = -5 + 7.25

; Fixed-point + Integer
.let mixed_add_5 = 2.5 + 5
.let mixed_add_6 = 0.75 + 10
.let mixed_add_7 = 3.14159 + 100
.let mixed_add_8 = 7.25 + (-5)

; Integer * Fixed-point
.let mixed_mult_1 = 3 * 2.5
.let mixed_mult_2 = 10 * 0.1
.let mixed_mult_3 = -4 * 1.5
.let mixed_mult_4 = 100 * 0.01

; Fixed-point * Integer
.let mixed_mult_5 = 2.5 * 3
.let mixed_mult_6 = 0.1 * 10
.let mixed_mult_7 = 1.5 * (-4)
.let mixed_mult_8 = 0.01 * 100

; Integer / Fixed-point
.let mixed_div_1 = 10 / 2.5
.let mixed_div_2 = 15 / 1.5
.let mixed_div_3 = 100 / 0.5
.let mixed_div_4 = -8 / 2.0

; Fixed-point / Integer
.let mixed_div_5 = 10.0 / 4
.let mixed_div_6 = 7.5 / 3
.let mixed_div_7 = 12.5 / 5
.let mixed_div_8 = -6.0 / 2

; ============================================================================
; EXPRESSIONS USING FIXED-POINT VARIABLES AND CONSTANTS
; ============================================================================

; Using mathematical constants
.let const_expr_1 = PI + E
.let const_expr_2 = PI * 2.0
.let const_expr_3 = E - 1.0
.let const_expr_4 = PI / TWO_PI

; Area and volume calculations
.let area_calc_1 = PI * 5.0 * 5.0
.let volume_calc_1 = QUARTER * PI * 10.0 * 10.0 * 10.0
.let circumference_calc_1 = TWO_PI * 3.0

; Using variables in arithmetic
.let var_expr_1 = fp_var_a + fp_var_b
.let var_expr_2 = fp_var_a * HALF
.let var_expr_3 = temperature - 32.0
.let var_expr_4 = voltage * 1000.0

; Complex expressions with mixed variables and constants
.let complex_expr_1 = (fp_var_a + fp_var_b) * PI
.let complex_expr_2 = SQRT_2 * (fp_var_a + fp_var_b)
.let complex_expr_3 = (temperature - 32.0) * (5.0 / 9.0)
.let complex_expr_4 = voltage / 3.3 * 100.0

; Variable reassignment and usage
.let radius = 5.0
.let circle_area = PI * radius * radius
.let circle_circumference = TWO_PI * radius

; Using derived variables
.let derived_expr_1 = circle_area / circle_circumference
.let derived_expr_2 = circle_circumference / (2.0 * radius)

; Fractions and percentages with constants
.let fraction_expr_1 = ONE_THIRD + TWO_THIRDS
.let fraction_expr_2 = THREE_QUARTERS - HALF
.let fraction_expr_3 = ONE_EIGHTH * 8.0
.let fraction_expr_4 = GOLDEN_RATIO - 1.0

; Temperature conversions
.let celsius = 25.0
.let fahrenheit = celsius * 9.0 / 5.0 + 32.0
.let kelvin = celsius + 273.15

; Using temperature variables
.let temp_expr_1 = fahrenheit - 32.0
.let temp_expr_2 = kelvin - 273.15
.let temp_expr_3 = (fahrenheit - 32.0) * 5.0 / 9.0

; Electrical calculations
.let resistance = 100.0
.let current = voltage / resistance
.let power = voltage * current

; Using electrical variables
.let elec_expr_1 = power * 1000.0
.let elec_expr_2 = current * 1000.0
.let elec_expr_3 = resistance / 1000.0

; ============================================================================
; COMPLEX EXPRESSIONS WITH PRECEDENCE
; ============================================================================

; Arithmetic precedence (multiplication before addition)
.let prec_expr_1 = 2.5 + 3.0 * 4.0
.let prec_expr_2 = 1.5 * 2.0 + 3.5
.let prec_expr_3 = 10.0 - 2.5 * 3.0
.let prec_expr_4 = 4.0 * 1.25 - 2.0

; Division precedence
.let div_prec_1 = 20.0 / 4.0 + 2.5
.let div_prec_2 = 15.0 + 12.0 / 3.0
.let div_prec_3 = 25.0 - 20.0 / 4.0
.let div_prec_4 = 8.0 / 2.0 * 3.0

; Parentheses override precedence
.let paren_expr_1 = (2.5 + 3.0) * 4.0
.let paren_expr_2 = 1.5 * (2.0 + 3.5)
.let paren_expr_3 = (10.0 - 2.5) * 3.0
.let paren_expr_4 = 4.0 * (1.25 - 2.0)

; Nested parentheses
.let nested_expr_1 = ((1.5 + 2.5) * 3.0) - 4.0
.let nested_expr_2 = 2.0 * ((3.5 + 1.5) / 2.0)
.let nested_expr_3 = ((10.0 / 2.0) + 3.0) * 1.5
.let nested_expr_4 = (5.0 - (2.5 + 1.0)) * 4.0

; ============================================================================
; NEGATIVE NUMBERS AND UNARY OPERATORS
; ============================================================================

; Negative fixed-point literals
.let neg_literal_1 = -3.14159
.let neg_literal_2 = -0.5
.let neg_literal_3 = -10.75
.let neg_literal_4 = -1.0

; Unary minus with expressions
.let unary_expr_1 = -(2.5 + 3.5)
.let unary_expr_2 = -(10.0 - 4.0)
.let unary_expr_3 = -(1.5 * 2.0)
.let unary_expr_4 = -(12.0 / 4.0)

; Double negatives
.let double_neg_1 = --3.14
.let double_neg_2 = --(-2.5)
.let double_neg_3 = -(-1.5)
.let double_neg_4 = --0.75

; Negative numbers in complex expressions
.let neg_complex_1 = -2.5 + 7.0
.let neg_complex_2 = 10.0 + (-3.5)
.let neg_complex_3 = -1.5 * 4.0
.let neg_complex_4 = (-6.0) / 2.0
.let neg_complex_5 = 2.0 * (-3.25)

; ============================================================================
; DECIMAL PRECISION EXAMPLES
; ============================================================================

; High precision calculations
.let precision_calc_1 = 3.141592653 + 2.718281828
.let precision_calc_2 = 1.41421356 * 1.73205080
.let precision_calc_3 = 2.236067977 - 1.618033988
.let precision_calc_4 = 3.14159 / 2.71828

; Small decimal values
.let small_decimal_1 = 0.001 + 0.002
.let small_decimal_2 = 0.0001 * 10000.0
.let small_decimal_3 = 0.1 + 0.01 + 0.001
.let small_decimal_4 = 1.0 - 0.999

; Large decimal values
.let large_decimal_1 = 999.999 + 0.001
.let large_decimal_2 = 1000.0 - 0.1
.let large_decimal_3 = 500.5 * 2.0
.let large_decimal_4 = 2000.25 / 4.0

; ============================================================================
; FRACTIONAL AND PERCENTAGE CALCULATIONS
; ============================================================================

; Common fractions as decimals
.let frac_decimal_1 = 0.25 + 0.5
.let frac_decimal_2 = 0.33333 + 0.66667
.let frac_decimal_3 = 0.125 * 8.0
.let frac_decimal_4 = 0.75 / 3.0

; Percentage calculations (treating as decimals)
.let percent_calc_1 = 100.0 * 0.15
.let percent_calc_2 = 250.0 * 0.08
.let percent_calc_3 = 1000.0 * 0.025
.let percent_calc_4 = 500.0 * 0.1

; Fraction arithmetic
.let fraction_1 = 1.0 / 3.0
.let fraction_2 = 2.0 / 3.0
.let fraction_3 = 1.0 / 4.0
.let fraction_4 = 3.0 / 4.0
.let fraction_5 = 1.0 / 8.0
.let fraction_6 = 5.0 / 8.0

; ============================================================================
; SCIENTIFIC AND MATHEMATICAL CONSTANTS
; ============================================================================

; Pi-related calculations
.let pi_calc_1 = 3.14159 * 2.0
.let pi_calc_2 = 3.14159 / 2.0
.let pi_calc_3 = 3.14159 * 3.14159
.let pi_calc_4 = 6.28318 / 2.0

; E-related calculations
.let e_calc_1 = 2.71828 + 1.0
.let e_calc_2 = 2.71828 * 2.0
.let e_calc_3 = 2.71828 / 1.41421

; Square roots approximations
.let sqrt_calc_1 = 1.41421 * 1.41421
.let sqrt_calc_2 = 1.73205 * 1.73205
.let sqrt_calc_3 = 2.23606 * 2.23606

; ============================================================================
; EXTREME VALUES AND EDGE CASES
; ============================================================================

; Very small numbers
.let small_num_1 = 0.0001 + 0.0002
.let small_num_2 = 0.00001 * 100000.0
.let small_num_3 = 0.1 / 1000.0

; Numbers close to integers
.let near_int_1 = 0.99999 + 0.00001
.let near_int_2 = 1.00001 - 0.00001
.let near_int_3 = 2.000001 * 2.0

; Large fixed-point numbers
.let large_num_1 = 12345.6789 + 9876.5432
.let large_num_2 = 99999.9 - 12345.6
.let large_num_3 = 10000.5 * 2.0
.let large_num_4 = 50000.25 / 2.0

; Zero operations
.let zero_op_1 = 0.0 + 5.5
.let zero_op_2 = 10.5 - 10.5
.let zero_op_3 = 0.0 * 999.999
.let zero_op_4 = 5.0 + 0.0
.let zero_op_5 = 7.25 * 1.0

; ============================================================================
; CHAINED OPERATIONS
; ============================================================================

; Long arithmetic chains
.let chain_add = 1.0 + 2.0 + 3.0 + 4.0 + 5.0
.let chain_sub = 10.0 - 1.0 - 2.0 - 3.0 - 1.0
.let chain_mult = 2.0 * 1.5 * 1.25 * 0.8
.let chain_div = 16.0 / 2.0 / 2.0 / 2.0

; Mixed operation chains
.let mixed_chain_1 = 1.5 + 2.5 * 3.0 - 4.0 / 2.0
.let mixed_chain_2 = 10.0 * 0.5 + 7.5 - 2.25
.let mixed_chain_3 = (1.0 + 2.0) * (3.0 - 1.5)
.let mixed_chain_4 = (8.0 / 2.0) + (6.0 * 1.5)

; Complex nested expressions
.let complex_nested_1 = ((1.5 + 2.5) * (3.0 - 1.0)) / 2.0
.let complex_nested_2 = (10.0 / (2.0 + 3.0)) * 4.0
.let complex_nested_3 = ((7.5 - 2.5) + (3.0 * 1.5)) / 2.0
.let complex_nested_4 = (((1.0 + 1.0) * 2.0) + 3.0) - 1.5

;
; The preprocessor should resolve the above expressions to their calculated
; fixed-point results. Examples of expected outputs:
;
; Basic arithmetic:
;   0.6      (0.2 + 0.4)
;   4.0      (1.5 + 2.5)
;   5.0      (3.25 + 1.75)
;   1.0      (0.1 + 0.9)
;   1.0      (1.5 - 0.5)
;   2.7      (5.0 - 2.3)
;   7.5      (10.75 - 3.25)
;   0.9      (1.0 - 0.1)
;   10.0     (2.5 * 4.0)
;   3.0      (1.5 * 2.0)
;   4.0      (0.5 * 8.0)
;   6.28     (3.14 * 2.0)
;   4.0      (10.0 / 2.5)
;   5.0      (7.5 / 1.5)
;   4.0      (12.0 / 3.0)
;   0.25     (1.0 / 4.0)
;
; Mixed operations:
;   7.5      (5 + 2.5)
;   10.75    (10 + 0.75)
;   103.14159 (100 + 3.14159)
;   2.25     (-5 + 7.25)
;   7.5      (3 * 2.5)
;   1.0      (10 * 0.1)
;   -6.0     (-4 * 1.5)
;   1.0      (100 * 0.01)
;   4.0      (10 / 2.5)
;   10.0     (15 / 1.5)
;   200.0    (100 / 0.5)
;   -4.0     (-8 / 2.0)
;
; Complex expressions:
;   14.5     (2.5 + 3.0 * 4.0)
;   6.5      (1.5 * 2.0 + 3.5)
;   2.5      (10.0 - 2.5 * 3.0)
;   3.0      (4.0 * 1.25 - 2.0)
;   22.0     ((2.5 + 3.0) * 4.0)
;   8.25     (1.5 * (2.0 + 3.5))
;   22.5     ((10.0 - 2.5) * 3.0)
;   -3.0     (4.0 * (1.25 - 2.0))
;
; Edge cases and special values are calculated according to the precision
; and representation used by the TM32 fixed-point number system.
;
; Variable and Constant Value Summary:
; ===================================
; PI = 3.14159
; E = 2.71828
; HALF = 0.5
; QUARTER = 0.25
; GOLDEN_RATIO = 1.618033988
; SQRT_2 = 1.41421356
; SQRT_3 = 1.73205080
; PI_OVER_2 = 1.5707963
; TWO_PI = 6.28318
; ONE_THIRD = 0.33333333
; TWO_THIRDS = 0.66666667
; THREE_QUARTERS = 0.75
; ONE_EIGHTH = 0.125
; fp_var_a = 2.5
; fp_var_b = 1.75
; fp_var_c = 0.0
; temperature = 98.6
; voltage = 3.3
; area = 12.56636 (PI * 2.0 * 2.0)
; circumference = 31.4159 (TWO_PI * 5.0)
; diagonal = 14.1421356 (SQRT_2 * 10.0)
; sum_value = 2.75 (0.0 + 1.5 * 2.0 - 0.25)
; radius = 5.0
; circle_area = 78.53975 (PI * radius * radius)
; circle_circumference = 31.4159 (TWO_PI * radius)
; celsius = 25.0
; fahrenheit = 77.0 (celsius * 9.0 / 5.0 + 32.0)
; kelvin = 298.15 (celsius + 273.15)
; resistance = 100.0
; current = 0.033 (voltage / resistance)
; power = 0.1089 (voltage * current)
;
; Expected Expression Results:
; ===========================
; Using mathematical constants:
;   5.85987    (PI + E)
;   6.28318    (PI * 2.0)
;   1.71828    (E - 1.0)
;   0.5        (PI / TWO_PI)
;
; Area and volume calculations:
;   78.53975   (PI * 5.0 * 5.0)
;   785.3975   (QUARTER * PI * 10.0 * 10.0 * 10.0)
;   18.84954   (TWO_PI * 3.0)
;
; Using variables in arithmetic:
;   4.25       (fp_var_a + fp_var_b)
;   1.25       (fp_var_a * HALF)
;   66.6       (temperature - 32.0)
;   3300.0     (voltage * 1000.0)
;
; Complex expressions:
;   13.35354   ((fp_var_a + fp_var_b) * PI)
;   6.010407   (SQRT_2 * (fp_var_a + fp_var_b))
;   37.0       ((temperature - 32.0) * (5.0 / 9.0))
;   100.0      (voltage / 3.3 * 100.0)
;
; Using derived variables:
;   2.5        (circle_area / circle_circumference)
;   3.14159    (circle_circumference / (2.0 * radius))
;
; Fractions and percentages:
;   1.0        (ONE_THIRD + TWO_THIRDS)
;   0.25       (THREE_QUARTERS - HALF)
;   1.0        (ONE_EIGHTH * 8.0)
;   0.618033988 (GOLDEN_RATIO - 1.0)
;
; Temperature conversions:
;   45.0       (fahrenheit - 32.0)
;   25.0       (kelvin - 273.15)
;   25.0       ((fahrenheit - 32.0) * 5.0 / 9.0)
;
; Electrical calculations:
;   108.9      (power * 1000.0)
;   33.0       (current * 1000.0)
;   0.1        (resistance / 1000.0)
;
