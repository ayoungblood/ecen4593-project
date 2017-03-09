# a stripped down assembly file, non-functional
# with disjoint gaps for testing parser address management

Disassembly of section .init:

00400548 <_init@@Base>:
  400548:	3c1c0002 	lui	gp,0x2
  40054c:	279c8ac8 	addiu	gp,gp,-30008
  400550:	0399e021 	addu	gp,gp,t9
  400554:	27bdffe0 	addiu	sp,sp,-32
  400558:	afbc0010 	sw	gp,16(sp)
  40055c:	afbf001c 	sw	ra,28(sp)

  400570:	0320f809 	jalr	t9
  400574:	00000000 	nop

  400580:	3c1c0042 	lui	gp,0x42

  400590:	04110076 	bal	40076c <_init@@Base+0x224>
  400594:	00000000 	nop

  40059c:	04110001 	bal	4005a4 <_init@@Base+0x5c>
  4005a0:	00000000 	nop
  4005a4:	3c1c0042 	lui	gp,0x42
  4005a8:	279c9010 	addiu	gp,gp,-28656
  4005ac:	8f998024 	lw	t9,-32732(gp)

Disassembly of section .fini:

00400970 <.fini>:
  400970:	3c1c0002 	lui	gp,0x2
  400974:	279c86a0 	addiu	gp,gp,-31072
  400978:	0399e021 	addu	gp,gp,t9
  40097c:	27bdffe0 	addiu	sp,sp,-32
  4009a0:	0411ff46 	bal	4006bc <_init@@Base+0x174>
  4009a4:	00000000 	nop
  4009a8:	8fbf001c 	lw	ra,28(sp)
  4009ac:	03e00008 	jr	ra
