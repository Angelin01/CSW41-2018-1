; potato.s
; Roda em TM4C1294
; Cont�m fun��o para movimenta��o de imagem para o Laborat�rio 1
; Angelo Borsoi Ross e Victor Barpp Gomes
; 12 de mar�o de 2018

width 	EQU 96
height 	EQU 64

		AREA |.text|, CODE, READONLY, ALIGN=2
		THUMB
		EXPORT mov_image
		
		EXTERN airplaneImage
		;EXTERN busImage
		EXTERN vel
		EXTERN aviao
		EXTERN sentidoNormal
		EXTERN buffer
		
;------------mov_image------------
; Movimenta a imagem que deve ser mostrada no display.
; Input: airplaneImage: ponteiro para a imagem do avi�o contida em airplane.c
;        busImage:      ponteiro para a imagem do �nibus contida em bus.c
;        vel:           velocidade da imagem (pixels por frame)
;        aviao:         se 1, movimenta o avi�o; sen�o, movimenta o �nibus
;        sentidoNormal: se 1, movimenta para cima ou esquerda; sen�o, movimenta para baixo ou direita
;        buffer:        espa�o em mem�ria de 5 bytes utilizado na movimenta��o
; Output: nenhum
; Modifica: R0, R1, R2, R3, R4, R5, R6, R7

mov_image
		PUSH {R0-R7}

		LDR R0, =aviao					; Carrega o ponteiro da vari�vel aviao
		LDRB R0, [R0]					; R0 = aviao
		CMP R0, #0
		BEQ eh_onibus					; Se aviao == 0, movimenta o onibus
										; Sen�o, movimenta o avi�o
; ======
; Avi�o
; ======
eh_aviao
		LDR R0, =sentidoNormal			; Carrega o ponteiro da vari�vel sentidoNormal
		LDRB R0, [R0]					; R0 = sentidoNormal
		CBZ R0, aviao_reverso			; Se sentidoNormal == 0, move no sentido reverso
										; Sen�o, movimenta no sentido normal

; ---------------------------
; Avi�o se movendo para cima
; ---------------------------
aviao_normal
		
		; =================
		; Loop externo
		MOV R2, #0						; R2 = posx = 0
aviao_normal_for_x
		
		; ------
		; Primeiro loop - Carrega buffer
		LDR R0, =airplaneImage			; R0 = airplaneImage
		LDR R1, =buffer					; R1 = buffer
		LDR R6, =vel
		LDR R6, [R6]					; R6 = vel
		MOV R3, #0						; R3 = posy = 0
		MOV R4, R2						; R4 = indice da imagem = posx + posy*largura
aviao_normal_preenche_buf
			LDRB R5, [R0, R4]				; R5 = airplaneImage[posx + posy*largura]
			STRB R5, [R1, R3]				; buffer[posy] = R5
			
			ADD R4, #width					; R4 += largura
			ADD R3, #1						; posy++
			CMP R3, R6
			BLT aviao_normal_preenche_buf	; Se posy < vel, vai para o pr�ximo item
											; Sen�o, come�a a mover pixels
		
		; ------
		; Segundo loop - move os restantes para cima
										; R3 j� tem o valor desejado (vel)
										; R4 j� tem o valor desejado (posx + vel*largura)
		MOV R7, R2						; R7 = �ndice destino (come�a em posx)
aviao_normal_move_pixels
			LDRB R5, [R0, R4]				; R5 = airplaneImage[posx + posy*largura]
			STRB R5, [R0, R7]				; airplaneImage[posx + (posy - vel)*largura] = R5
			
			ADD R4, #width					; R4 += largura
			ADD R7, #width					; R7 += largura
			ADD R3, #1						; posy++
			CMP R3, #height
			BLT aviao_normal_move_pixels	; Se posy < altura, vai para o pr�ximo item
											; Sen�o, come�a a recolocar o buffer
		
		; ------
		; Terceiro loop - recoloca o buffer no final
		SUB R3, R6						; R3 = altura - vel
		MOV R7, #width					; tempor�rio para multiplicar
		MLA R4, R3, R7, R2				; R4 = (altura - vel)*largura + posx
		MOV R7, #0						; R7 = 0 (primeira pos do buffer)
aviao_normal_restaura_buf
			LDRB R5, [R1, R7]				; R5 = buffer[i]
			STRB R5, [R0, R4]				; airplaneImage[posx + posy*largura] = R5
			
			ADD R4, #width					; R4 += largura
			ADD R7, #1						; i++
			ADD R3, #1						; posy++
			CMP R3, #height
			BLT aviao_normal_restaura_buf	; Se posy < altura, vai para o pr�ximo item
											; Sen�o, vai para a pr�xima coluna
		
		; =================
		; Fim loop externo
		ADD R2, #1						; posx++
		CMP R2, #width
		BLT aviao_normal_for_x			; Se posx < largura, vai para a pr�xima coluna
		B vazar							; Sen�o, finaliza a fun��o

; ----------------------------
; Avi�o se movendo para baixo
; ----------------------------
aviao_reverso

		; =================
		; Loop externo
		MOV R2, #0						; R2 = posx = 0
aviao_reverso_for_x
		
		; ------
		; Primeiro loop - Carrega buffer
		LDR R0, =airplaneImage			; R0 = airplaneImage
		LDR R1, =buffer					; R1 = buffer
		LDR R6, =vel
		LDR R6, [R6]					; R6 = vel
		MOV R3, #height
		SUB R3, R6						; R3 = posy = altura - vel
		MOV R7, #width					; tempor�rio para multiplicar
		MLA R4, R3, R7, R2				; R4 = posy*largura + posx
		MOV R7, #0						; R7 = i = 0
aviao_reverso_preenche_buf
			LDRB R5, [R0, R4]				; R5 = airplaneImage[posx + posy*largura]
			STRB R5, [R1, R7]				; buffer[i] = R5
			
			ADD R3, #1						; posy++
			ADD R7, #1						; i++
			ADD R4, #width					; R4 += largura
			CMP R3, #height
			BLT aviao_reverso_preenche_buf	; Se posy < altura, vai para o pr�ximo item
											; Sen�o, come�a a mover pixels
		
		; ------
		; Segundo loop - move os restantes para baixo
		SUB R3, #1
		SUB R3, R6						; R3 = posy = (altura-1) - vel
		SUB R4, #width					; R4 = posx + (posy + vel)*largura (destino)
		MOV R7, #width					; tempor�rio para multiplicar
		MLA R7, R3, R7, R2				; R7 = posy*largura + posx (origem)
aviao_reverso_move_pixels
			LDRB R5, [R0, R7]				; R5 = airplaneImage[posx + posy*largura]
			STRB R5, [R0, R4]				; airplaneImage[posx + (posy + vel)*largura] = R5
			
			SUB R4, #width					; R4 -= largura
			SUB R7, #width					; R7 -= largura
			SUB R3, #1						; R3 --
			CMP R3, #0
			BGE aviao_reverso_move_pixels	; Se posy >= 0, vai para o pr�ximo item
											; Sen�o, come�a a recolocar o buffer
		
		; ------
		; Terceiro loop - recoloca o buffer no come�o
		MOV R3, #0						; R3 = posy = 0
		MOV R4, R2						; R4 = posx + posy*largura = posx
aviao_reverso_restaura_buf
			LDRB R5, [R1, R3]				; R5 = buffer[posy]
			STRB R5, [R0, R4]				; airplaneImage[posx + posy*largura] = R5
			
			ADD R4, #width					; R4 += largura
			ADD R3, #1						; R3 ++
			CMP R3, R6
			BLT aviao_reverso_restaura_buf	; Se posy < vel, vai para o pr�ximo item
											; Sen�o, vai para a pr�xima coluna
		
		; =================
		; Fim loop externo
		ADD R2, #1						; posx++
		CMP R2, #width
		BLT aviao_reverso_for_x			; Se posx < largura, vai para a pr�xima coluna
		B vazar							; Sen�o, finaliza a fun��o


; =======
; �nibus
; =======
eh_onibus
		LDR R0, =sentidoNormal			; Carrega o ponteiro da vari�vel sentidoNormal
		LDR R0, [R0]					; R0 = sentidoNormal
		CBZ R0, onibus_reverso			; Se sentidoNormal == 0, move no sentido reverso
										; Sen�o, movimenta no sentido normal

; ----------------------------
; �nibus se movendo para cima
; ----------------------------
onibus_normal
		
		MOV R3, #0						; R3 = posy = 0
onibus_normal_for_y
		; @TODO
		; Conte�do do for
		ADD R3, #1						; posy++
		CMP R3, #height
		BLT onibus_normal_for_y			; Se posy < altura, vai para a pr�xima linha
		B vazar							; Sen�o, finaliza a fun��o

; -----------------------------
; �nibus se movendo para baixo
; -----------------------------
onibus_reverso

		MOV R3, #0						; R3 = posy = 0
onibus_reverso_for_y
		; @TODO
		; Conte�do do for
		ADD R3, #1						; posy++
		CMP R3, #height
		BLT onibus_reverso_for_y		; Se posy < altura, vai para a pr�xima linha
										; Sen�o, finaliza a fun��o

vazar
		POP {R0-R7}
		BX LR
		
		ALIGN
		END
