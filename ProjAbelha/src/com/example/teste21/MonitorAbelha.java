package com.example.teste21;

import java.net.SocketException;
import java.util.LinkedList;

import ufrj.coppe.lcp.repa.PrefixAddress;
import ufrj.coppe.lcp.repa.RepaMessage;
import ufrj.coppe.lcp.repa.RepaSocket;

public class MonitorAbelha {
	public MonitorAbelha(){
		RepaSocket rpa = RepaSocket.getRepaSocket();
		try {
			rpa.repaOpen();
			rpa.registerInterest("apt-chat://message");
			rpa.repaSend(new RepaMessage("app-chat://message","CAGE GATÃO!".getBytes(),new PrefixAddress()));
			rpa.repaClose();
		} catch (SocketException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	LinkedList<String> listaReq = new LinkedList<String>();
	// Gera dado randomico
	private int gerarDadoTempetura(){
		return (int) (Math.random()*100);
	}
	
	
	// Recebe um string e manda pro servidor.
	public void sendMsgToServer(){
		
	}
	
	// PARSE da requisição
	private void getData(){
		
	}
	
	// Thread que pega fila e executa
	private void trataPedidos(){
		
	}
	
	// Thread que ouve servidor e infilera pedido | EU 
	public void listenerMsg(){
		
	}
}
