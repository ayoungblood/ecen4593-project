# Modified example code from Professor Graham

00400000 : <text>
400000:	0x201d0fa0		//   $sp = 4000
400004:	0x201e0fa0		//   $fp = 4000
400008:	0x10000003      //   beq $zero, $zero, 0x400018
40000c:	0x00000000
400010:	0x00000000
400014:	0x00000032		//
400018:	0x00000000		//
40001c:	0x00000000		//
400020:	0x00000000		//
400024:	0x00000000		//
400028:	0x00001025      	move	v0,zero    <load_arrays>:
400038:	0xac830000      	sw	v1,0(a0)
40003c:	0x8ca30000      	lw	v1,0(a1)
400040:	0x00621821      	addu	v1,v1,v0
400044:	0xaca30000      	sw	v1,0(a1)
400048:	0x24420001      	addiu	v0,v0,1
40004c:	0x24840004      	addiu	a0,a0,4
400050:	0x1446fff7      	bne	v0,a2,400838 <load_arrays+0x8>
400054:	0x24a50004      	addiu	a1,a1,4
400058:	0x8d090f90		//lw $t1, 3576($t0)
40005c:	0xad090018		//sw $t1, 24($t0)
400060:	0x8d080418		//lw $t0, 1048($t0)
400064:	0x03e00008      	jr	ra
400068:	0xac030018      	sw v1, 24($zer0)
40006c:	0x00000000      	nop
400070:	0x00000000      	nop
400074:	0x00000000      	nop
400078:	0x00801825      	move	v1,a0
40007c:	0x24870190     	addiu	a3,a0,400
400080:	0x00001025     	move	v0,zero
400084:	0x24080001     	li	t0,1
400088:	0x8c640000     	lw	a0,0(v1)
40008c:	0x8ca60000     	lw	a2,0(a1)
400090:	0x00862026     	xor	a0,a0,a2
400094:	0x0104100b     	movn	v0,t0,a0
400098:	0x24630004     	addiu	v1,v1,4
40009c:	0x1467fffa     	bne	v1,a3,400878 <check_result+0x10>
4000a0:	0x24a50004     	addiu	a1,a1,4
4000a4:	0xac02001c		//sw $v0, 28($zero)
4000a8:	0x8c18041c		//lw $t8, 1052($zero)
4000ac:	0xac030020		//sw $v1, 32($zero)
4000b0:	0x8c180420		//lw $t8, 1056($zero)
4000b4:	0x03e00008     	jr	ra
4000b8:	0x00000000     	nop
4000bc:	0x00000000     	nop
4000c0:	0x00000000     	nop
4000c4:	0x00000000     	nop
4000c8:	0x27bdfcb0     	addiu	sp,sp,-848   <main>:
4000cc:	0xafbf034c     	sw	ra,844(sp)
4000d0:	0xafb20348     	sw	s2,840(sp)
4000d4:	0xafb10344     	sw	s1,836(sp)
4000d8:	0xafb00340     	sw	s0,832(sp)
4000dc:	0xafa2033c     	sw	v0,828(sp)
4000e0:	0x27b101ac     	addiu	s1,sp,428
4000e4:	0x02202825     	move	a1,s1
4000e8:	0x27b0001c     	addiu	s0,sp,28
4000ec:	0x02002025     	move	a0,s0
4000f0:	0x0c00000a     	jal	10 <load_arrays>
4000f4:	0x00000000     	nop
4000f8:	0x02202825     	move	a1,s1
4000fc:	0x02002025     	move	a0,s0
400100:	0x0c00001e     	jal	30 <check_result>
400104:	0x00000000     	nop
400108:	0x8fa4033c     	lw	a0,828(sp)
40010c:	0x8e430000     	lw	v1,0(s2)
400110:	0x10830004     	beq	a0,v1,400914 <main+0x78>
400114:	0x8fbf034c     	lw	ra,844(sp)
400118:	0x00000000     	nop
40011c:	0x8fb20348     	lw	s2,840(sp)
400120:	0x8fb10344     	lw	s1,836(sp)
400124:	0x8fb00340     	lw	s0,832(sp)
400128:	0x00000008     	jr	$zero
40012c:	0x00000000	   //
400130:	0x00000000


00401000: <data>
  401000:	fa5f0000
  401004:	fa5f0044
  401008:	fa5f0088
  40100c:	fa5f00cc
  401010:	1ff12ff2
  401014:	3ff34ff4
  401018:	11223344
  40101c:	55667788
  401020:	00000000
  401024:	00000000
  401028:	00000000
  40102c:	00000000
  401030:	00000000
  401034:	00000000
  401038:	00000000
  40103c:	00000000
  401040:	00000000
  401044:	00000000
  401048:	00000000
  40104c:	00000000
  401050:	00000000
  401054:	00000000
  401058:	00000000
  40105c:	00000000
  401060:	00000000
  401064:	00000000
  401068:	00000000
  40106c:	00000000
  401070:	00000000
  401074:	00000000
  401078:	00000000
  40107c:	00000000
