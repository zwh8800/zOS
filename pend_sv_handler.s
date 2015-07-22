.syntax unified
.cpu cortex-m3
.fpu softvfp
.eabi_attribute 20, 1
.eabi_attribute 21, 1
.eabi_attribute 23, 3
.eabi_attribute 24, 1
.eabi_attribute 25, 1
.eabi_attribute 26, 1
.eabi_attribute 30, 6
.eabi_attribute 34, 1
.eabi_attribute 18, 4
.thumb
.text

.align	2
.global PendSV_Handler
.thumb_func
.type	PendSV_Handler, %function
PendSV_Handler:
	ldr r0, =prev_proc
	ldr r0, [r0]
	cbz r0, skip1
	mrs r1, psp
	stmia r0, {r1, r4-r11}
skip1:
	ldr r0, =current
	ldr r0, [r0]
	cbz r0, skip2
	ldmia r0, {r1, r4-r11}
	msr psp, r1
skip2:
	ldr lr, =0xfffffffd	@thread mode psp
	bx lr
