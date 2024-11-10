################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
libraries/ti/devices/msp432p4xx/driverlib/%.obj: ../libraries/ti/devices/msp432p4xx/driverlib/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccs1220/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/ti/ccs1220/ccs/ccs_base/arm/include" --include_path="C:/ti/ccs1220/ccs/ccs_base/arm/include/CMSIS" --include_path="C:/Users/victor.apeh/Documents/year I/Embedded Systems/Project/project/project" --include_path="C:/ti/ccs1220/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/include" --include_path="C:/Users/victor.apeh/Documents/year I/Embedded Systems/Project/project/project/libraries" --include_path="C:/Users/victor.apeh/Documents/year I/Embedded Systems/Project/project/project/libraries/bosch/bmi160" --advice:power=all --define=__MSP432P401R__ --define=ccs -g --c11 --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="libraries/ti/devices/msp432p4xx/driverlib/$(basename $(<F)).d_raw" --obj_directory="libraries/ti/devices/msp432p4xx/driverlib" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


