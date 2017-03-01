
a.out:     file format elf32-tradbigmips


Disassembly of section .init:

00400548 <_init@@Base>:
  400548:	3c1c0002 	lui	gp,0x2
  40054c:	279c8ac8 	addiu	gp,gp,-30008
  400550:	0399e021 	addu	gp,gp,t9
  400554:	27bdffe0 	addiu	sp,sp,-32
  400558:	afbc0010 	sw	gp,16(sp)
  40055c:	afbf001c 	sw	ra,28(sp)
  400560:	8f828050 	lw	v0,-32688(gp)
  400564:	10400004 	beqz	v0,400578 <_init@@Base+0x30>
  400568:	00000000 	nop
  40056c:	8f998050 	lw	t9,-32688(gp)
  400570:	0320f809 	jalr	t9
  400574:	00000000 	nop
  400578:	04110001 	bal	400580 <_init@@Base+0x38>
  40057c:	00000000 	nop
  400580:	3c1c0042 	lui	gp,0x42
  400584:	279c9010 	addiu	gp,gp,-28656
  400588:	8f998024 	lw	t9,-32732(gp)
  40058c:	2739076c 	addiu	t9,t9,1900
  400590:	04110076 	bal	40076c <_init@@Base+0x224>
  400594:	00000000 	nop
  400598:	8fbc0010 	lw	gp,16(sp)
  40059c:	04110001 	bal	4005a4 <_init@@Base+0x5c>
  4005a0:	00000000 	nop
  4005a4:	3c1c0042 	lui	gp,0x42
  4005a8:	279c9010 	addiu	gp,gp,-28656
  4005ac:	8f998024 	lw	t9,-32732(gp)
  4005b0:	273908f0 	addiu	t9,t9,2288
  4005b4:	041100ce 	bal	4008f0 <__libc_csu_fini@@Base+0xc>
  4005b8:	00000000 	nop
  4005bc:	8fbf001c 	lw	ra,28(sp)
  4005c0:	03e00008 	jr	ra
  4005c4:	27bd0020 	addiu	sp,sp,32

Disassembly of section .text:

004005d0 <main@@Base-0x1f0>:
  4005d0:	03e00025 	move	zero,ra
  4005d4:	04110001 	bal	4005dc <_init@@Base+0x94>
  4005d8:	00000000 	nop
  4005dc:	3c1c0042 	lui	gp,0x42
  4005e0:	279c9010 	addiu	gp,gp,-28656
  4005e4:	0000f825 	move	ra,zero
  4005e8:	8f848018 	lw	a0,-32744(gp)
  4005ec:	8fa50000 	lw	a1,0(sp)
  4005f0:	27a60004 	addiu	a2,sp,4
  4005f4:	2401fff8 	li	at,-8
  4005f8:	03a1e824 	and	sp,sp,at
  4005fc:	27bdffe0 	addiu	sp,sp,-32
  400600:	8f87801c 	lw	a3,-32740(gp)
  400604:	8f888020 	lw	t0,-32736(gp)
  400608:	afa80010 	sw	t0,16(sp)
  40060c:	afa20014 	sw	v0,20(sp)
  400610:	afbd0018 	sw	sp,24(sp)
  400614:	8f99804c 	lw	t9,-32692(gp)
  400618:	0320f809 	jalr	t9
  40061c:	00000000 	nop
  400620:	1000ffff 	b	400620 <_init@@Base+0xd8>
  400624:	00000000 	nop
	...
  400630:	3c040041 	lui	a0,0x41
  400634:	3c020041 	lui	v0,0x41
  400638:	24841014 	addiu	a0,a0,4116
  40063c:	24421017 	addiu	v0,v0,4119
  400640:	00441023 	subu	v0,v0,a0
  400644:	2c420007 	sltiu	v0,v0,7
  400648:	14400007 	bnez	v0,400668 <_init@@Base+0x120>
  40064c:	3c1c0042 	lui	gp,0x42
  400650:	279c9010 	addiu	gp,gp,-28656
  400654:	8f998054 	lw	t9,-32684(gp)
  400658:	13200003 	beqz	t9,400668 <_init@@Base+0x120>
  40065c:	00000000 	nop
  400660:	03200008 	jr	t9
  400664:	00000000 	nop
  400668:	03e00008 	jr	ra
  40066c:	00000000 	nop
  400670:	3c040041 	lui	a0,0x41
  400674:	3c020041 	lui	v0,0x41
  400678:	24841014 	addiu	a0,a0,4116
  40067c:	24421014 	addiu	v0,v0,4116
  400680:	00441023 	subu	v0,v0,a0
  400684:	00021083 	sra	v0,v0,0x2
  400688:	00022fc2 	srl	a1,v0,0x1f
  40068c:	00a22821 	addu	a1,a1,v0
  400690:	00052843 	sra	a1,a1,0x1
  400694:	10a00007 	beqz	a1,4006b4 <_init@@Base+0x16c>
  400698:	3c1c0042 	lui	gp,0x42
  40069c:	279c9010 	addiu	gp,gp,-28656
  4006a0:	8f998044 	lw	t9,-32700(gp)
  4006a4:	13200003 	beqz	t9,4006b4 <_init@@Base+0x16c>
  4006a8:	00000000 	nop
  4006ac:	03200008 	jr	t9
  4006b0:	00000000 	nop
  4006b4:	03e00008 	jr	ra
  4006b8:	00000000 	nop
  4006bc:	27bdffd0 	addiu	sp,sp,-48
  4006c0:	afb30028 	sw	s3,40(sp)
  4006c4:	3c130041 	lui	s3,0x41
  4006c8:	afbf002c 	sw	ra,44(sp)
  4006cc:	afb20024 	sw	s2,36(sp)
  4006d0:	afb10020 	sw	s1,32(sp)
  4006d4:	afb0001c 	sw	s0,28(sp)
  4006d8:	92621080 	lbu	v0,4224(s3)
  4006dc:	1440001c 	bnez	v0,400750 <_init@@Base+0x208>
  4006e0:	3c110041 	lui	s1,0x41
  4006e4:	3c020041 	lui	v0,0x41
  4006e8:	26310ff4 	addiu	s1,s1,4084
  4006ec:	24420ff8 	addiu	v0,v0,4088
  4006f0:	3c100041 	lui	s0,0x41
  4006f4:	00518823 	subu	s1,v0,s1
  4006f8:	3c020041 	lui	v0,0x41
  4006fc:	00118883 	sra	s1,s1,0x2
  400700:	24520ff4 	addiu	s2,v0,4084
  400704:	8e021084 	lw	v0,4228(s0)
  400708:	2631ffff 	addiu	s1,s1,-1
  40070c:	0051182b 	sltu	v1,v0,s1
  400710:	1060000b 	beqz	v1,400740 <_init@@Base+0x1f8>
  400714:	24420001 	addiu	v0,v0,1
  400718:	00021880 	sll	v1,v0,0x2
  40071c:	ae021084 	sw	v0,4228(s0)
  400720:	02431021 	addu	v0,s2,v1
  400724:	8c590000 	lw	t9,0(v0)
  400728:	0320f809 	jalr	t9
  40072c:	00000000 	nop
  400730:	8e021084 	lw	v0,4228(s0)
  400734:	0051182b 	sltu	v1,v0,s1
  400738:	1460fff7 	bnez	v1,400718 <_init@@Base+0x1d0>
  40073c:	24420001 	addiu	v0,v0,1
  400740:	0c10018c 	jal	400630 <_init@@Base+0xe8>
  400744:	00000000 	nop
  400748:	24020001 	li	v0,1
  40074c:	a2621080 	sb	v0,4224(s3)
  400750:	8fbf002c 	lw	ra,44(sp)
  400754:	8fb30028 	lw	s3,40(sp)
  400758:	8fb20024 	lw	s2,36(sp)
  40075c:	8fb10020 	lw	s1,32(sp)
  400760:	8fb0001c 	lw	s0,28(sp)
  400764:	03e00008 	jr	ra
  400768:	27bd0030 	addiu	sp,sp,48
  40076c:	3c020041 	lui	v0,0x41
  400770:	24440ffc 	addiu	a0,v0,4092
  400774:	8c820000 	lw	v0,0(a0)
  400778:	14400003 	bnez	v0,400788 <_init@@Base+0x240>
  40077c:	3c1c0042 	lui	gp,0x42
  400780:	0810019c 	j	400670 <_init@@Base+0x128>
  400784:	00000000 	nop
  400788:	27bdffe0 	addiu	sp,sp,-32
  40078c:	279c9010 	addiu	gp,gp,-28656
  400790:	afbf001c 	sw	ra,28(sp)
  400794:	afbc0010 	sw	gp,16(sp)
  400798:	8f998048 	lw	t9,-32696(gp)
  40079c:	13200005 	beqz	t9,4007b4 <_init@@Base+0x26c>
  4007a0:	8fbf001c 	lw	ra,28(sp)
  4007a4:	0320f809 	jalr	t9
  4007a8:	00000000 	nop
  4007ac:	8fbc0010 	lw	gp,16(sp)
  4007b0:	8fbf001c 	lw	ra,28(sp)
  4007b4:	0810019c 	j	400670 <_init@@Base+0x128>
  4007b8:	27bd0020 	addiu	sp,sp,32
  4007bc:	00000000 	nop

004007c0 <main@@Base>:
  4007c0:	27bdfff8 	addiu	sp,sp,-8
  4007c4:	afbe0004 	sw	s8,4(sp)
  4007c8:	03a0f025 	move	s8,sp
  4007cc:	3c1c0042 	lui	gp,0x42
  4007d0:	279c9010 	addiu	gp,gp,-28656
  4007d4:	8f828028 	lw	v0,-32728(gp)
  4007d8:	ac400000 	sw	zero,0(v0)
  4007dc:	1000000d 	b	400814 <main@@Base+0x54>
  4007e0:	00000000 	nop
  4007e4:	8f82802c 	lw	v0,-32724(gp)
  4007e8:	8c430000 	lw	v1,0(v0)
  4007ec:	8f828028 	lw	v0,-32728(gp)
  4007f0:	8c420000 	lw	v0,0(v0)
  4007f4:	00621821 	addu	v1,v1,v0
  4007f8:	8f82802c 	lw	v0,-32724(gp)
  4007fc:	ac430000 	sw	v1,0(v0)
  400800:	8f828028 	lw	v0,-32728(gp)
  400804:	8c420000 	lw	v0,0(v0)
  400808:	24430001 	addiu	v1,v0,1
  40080c:	8f828028 	lw	v0,-32728(gp)
  400810:	ac430000 	sw	v1,0(v0)
  400814:	8f828028 	lw	v0,-32728(gp)
  400818:	8c420000 	lw	v0,0(v0)
  40081c:	2842000a 	slti	v0,v0,10
  400820:	1440fff0 	bnez	v0,4007e4 <main@@Base+0x24>
  400824:	00000000 	nop
  400828:	00001025 	move	v0,zero
  40082c:	03c0e825 	move	sp,s8
  400830:	8fbe0004 	lw	s8,4(sp)
  400834:	27bd0008 	addiu	sp,sp,8
  400838:	03e00008 	jr	ra
  40083c:	00000000 	nop

00400840 <__libc_csu_init@@Base>:
  400840:	3c1c0002 	lui	gp,0x2
  400844:	279c87d0 	addiu	gp,gp,-30768
  400848:	0399e021 	addu	gp,gp,t9
  40084c:	27bdffc8 	addiu	sp,sp,-56
  400850:	8f998030 	lw	t9,-32720(gp)
  400854:	afbc0010 	sw	gp,16(sp)
  400858:	afb50030 	sw	s5,48(sp)
  40085c:	00c0a825 	move	s5,a2
  400860:	afb4002c 	sw	s4,44(sp)
  400864:	00a0a025 	move	s4,a1
  400868:	afb30028 	sw	s3,40(sp)
  40086c:	00809825 	move	s3,a0
  400870:	afb20024 	sw	s2,36(sp)
  400874:	afb0001c 	sw	s0,28(sp)
  400878:	afbf0034 	sw	ra,52(sp)
  40087c:	0411ff32 	bal	400548 <_init@@Base>
  400880:	afb10020 	sw	s1,32(sp)
  400884:	8fbc0010 	lw	gp,16(sp)
  400888:	8f908034 	lw	s0,-32716(gp)
  40088c:	8f928034 	lw	s2,-32716(gp)
  400890:	02509023 	subu	s2,s2,s0
  400894:	00129083 	sra	s2,s2,0x2
  400898:	12400009 	beqz	s2,4008c0 <__libc_csu_init@@Base+0x80>
  40089c:	00008825 	move	s1,zero
  4008a0:	8e190000 	lw	t9,0(s0)
  4008a4:	26310001 	addiu	s1,s1,1
  4008a8:	02a03025 	move	a2,s5
  4008ac:	02802825 	move	a1,s4
  4008b0:	0320f809 	jalr	t9
  4008b4:	02602025 	move	a0,s3
  4008b8:	1632fff9 	bne	s1,s2,4008a0 <__libc_csu_init@@Base+0x60>
  4008bc:	26100004 	addiu	s0,s0,4
  4008c0:	8fbf0034 	lw	ra,52(sp)
  4008c4:	8fb50030 	lw	s5,48(sp)
  4008c8:	8fb4002c 	lw	s4,44(sp)
  4008cc:	8fb30028 	lw	s3,40(sp)
  4008d0:	8fb20024 	lw	s2,36(sp)
  4008d4:	8fb10020 	lw	s1,32(sp)
  4008d8:	8fb0001c 	lw	s0,28(sp)
  4008dc:	03e00008 	jr	ra
  4008e0:	27bd0038 	addiu	sp,sp,56

004008e4 <__libc_csu_fini@@Base>:
  4008e4:	03e00008 	jr	ra
  4008e8:	00000000 	nop
  4008ec:	00000000 	nop
  4008f0:	3c030041 	lui	v1,0x41
  4008f4:	2402ffff 	li	v0,-1
  4008f8:	8c790fec 	lw	t9,4076(v1)
  4008fc:	1322000f 	beq	t9,v0,40093c <__libc_csu_fini@@Base+0x58>
  400900:	00000000 	nop
  400904:	27bdffd8 	addiu	sp,sp,-40
  400908:	afb10020 	sw	s1,32(sp)
  40090c:	2411ffff 	li	s1,-1
  400910:	afb0001c 	sw	s0,28(sp)
  400914:	24700fec 	addiu	s0,v1,4076
  400918:	afbf0024 	sw	ra,36(sp)
  40091c:	0320f809 	jalr	t9
  400920:	2610fffc 	addiu	s0,s0,-4
  400924:	8e190000 	lw	t9,0(s0)
  400928:	1731fffc 	bne	t9,s1,40091c <__libc_csu_fini@@Base+0x38>
  40092c:	8fbf0024 	lw	ra,36(sp)
  400930:	8fb10020 	lw	s1,32(sp)
  400934:	8fb0001c 	lw	s0,28(sp)
  400938:	27bd0028 	addiu	sp,sp,40
  40093c:	03e00008 	jr	ra
  400940:	00000000 	nop
	...

Disassembly of section .MIPS.stubs:

00400950 <.MIPS.stubs>:
  400950:	8f998010 	lw	t9,-32752(gp)
  400954:	03e07825 	move	t7,ra
  400958:	0320f809 	jalr	t9
  40095c:	2418000c 	li	t8,12
	...

Disassembly of section .fini:

00400970 <.fini>:
  400970:	3c1c0002 	lui	gp,0x2
  400974:	279c86a0 	addiu	gp,gp,-31072
  400978:	0399e021 	addu	gp,gp,t9
  40097c:	27bdffe0 	addiu	sp,sp,-32
  400980:	afbc0010 	sw	gp,16(sp)
  400984:	afbf001c 	sw	ra,28(sp)
  400988:	04110001 	bal	400990 <__libc_csu_fini@@Base+0xac>
  40098c:	00000000 	nop
  400990:	3c1c0042 	lui	gp,0x42
  400994:	279c9010 	addiu	gp,gp,-28656
  400998:	8f998024 	lw	t9,-32732(gp)
  40099c:	273906bc 	addiu	t9,t9,1724
  4009a0:	0411ff46 	bal	4006bc <_init@@Base+0x174>
  4009a4:	00000000 	nop
  4009a8:	8fbf001c 	lw	ra,28(sp)
  4009ac:	03e00008 	jr	ra
  4009b0:	27bd0020 	addiu	sp,sp,32
