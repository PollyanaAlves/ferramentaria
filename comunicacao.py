#!/usr/bin/python
# -*- coding: iso-8859-1 -*-
import time
import serial
comport = serial.Serial('COM5', 9600) 
print ('Serial Iniciada...\n')
 
import mysql.connector
cnx = mysql.connector.connect(user='root', password='root', host='127.0.0.1', database='ferramentaria')
cursor = cnx.cursor()

while (True):
    fo = open("comunicacao.txt", "r")
    tabela = fo.read()
    fo.close()

    if tabela != '':
        if tabela == "fer":
            time.sleep(1)
            comport.write(b'1')

            serialValue = comport.readline()
            data_dados = serialValue.split(b"|")
            print(data_dados)
            cursor.execute("INSERT INTO ferramentas (tagID, localizacao) VALUES (%s, %s)", data_dados)

        if tabela == "mov":
            time.sleep(1)
            comport.write(b'2')

            serialValue = comport.readline()
            data_dados = serialValue.split(b"|")
            print(data_dados)
            cursor.execute("INSERT INTO movimentacao (tagID, localizacao) VALUES (%s, %s)", data_dados)

        fo = open("comunicacao.txt", "w")
        fo.close()

    fo2 = open("comunicacao2.txt", "r")
    local = fo2.read()
    fo2.close()

    if local != '':
        if local == "2":
            time.sleep(1)
            comport.write(b'3')
            
            confirmacao = comport.readline()
            (confirmacao2, confirmacao3) = confirmacao.split(b"|")
            
            encoding = 'utf-8'
            conf = str(confirmacao2, encoding)
            
            if conf == "ok":
                fo3 = open("confirmacao.txt", "w")
                fo3.write("1")
                fo3.close()
            
        if local == "3":
            time.sleep(1)
            comport.write(b'4')

            confirmacao = comport.readline()
            (confirmacao2, confirmacao3) = confirmacao.split(b"|")
            
            encoding = 'utf-8'
            conf = str(confirmacao2, encoding)
            
            if conf == "ok":
                fo3 = open("confirmacao.txt", "w")
                fo3.write("1")
                fo3.close()
            
        if local == "4":
            time.sleep(1)
            comport.write(b'5')

            confirmacao = comport.readline()
            (confirmacao2, confirmacao3) = confirmacao.split(b"|")
            
            encoding = 'utf-8'
            conf = str(confirmacao2, encoding)
            
            if conf == "ok":
                fo3 = open("confirmacao.txt", "w")
                fo3.write("1")
                fo3.close()
            
        fo2 = open("comunicacao2.txt", "w")
        fo2.close()

    fo4 = open("condicao.txt", "r")
    condicao = fo4.read()
    fo4.close()

    if condicao != '':
        if condicao == "quar":
            cond = comport.readline()
            (cond2, cond3) = cond.split(b"|")
                
            encoding = 'utf-8'
            condi = str(cond2, encoding)

            if condi == "quar":
                fo5 = open("condicao2.txt", "w")
                fo5.write("1")
                fo5.close()

            if condi == "cali":
                fo5 = open("condicao2.txt", "w")
                fo5.write("2")
                fo5.close()

        fo4 = open("condicao.txt", "w")
        fo4.close()

    cnx.commit()

cursor.close()
cnx.close()
comport.close()
