# WIFI

## Descripción
La siguiente guía tiene como finalidad configurar la Raspberry Pi como Access Point y servidor DHCP, con el fin de que los sensores la utilicen para conectarse de forma inalámbrica para poder tener 2 métodos de comunicación para enviar sus datos (TTN y mediante SNMP vía wifi).

## Procedimiento de configuración de Access Point con servidor DHCP sobre Raspberry Pi

1. Instalar software para Access Point y servidor DHCP

	**sudo apt install hostapd isc-dhcp-server**


2. Configurar interfaces de red:

	**sudo vim /etc/network/interfaces**
	
	```
	auto eth0
	iface eth0 inet dhcp
	
	auto wlan0
	allow-hotplug wlan0
	iface wlan0 inet static
	  address 10.42.66.1
	  netmask 255.255.255.0
	```

3. Configurar interfaz de red para el servidor DHCP

	**sudo vim /etc/default/isc-dhcp-server**
	
	```
	INTERFACESv4="wlan0"
	```

4. Configurar servidor DHCP

	**sudo vim /etc/dhcp/dhcpd.conf**
	
	```
	option domain-name "localdomain.com";
	default-lease-time 600;
	max-lease-time 7200;
	ddns-update-style none;
	authoritative;
	subnet 10.42.66.0 netmask 255.255.255.0 {
	  range 10.42.66.10 10.42.66.50;
	  option routers 10.42.66.1;
	  option domain-name-servers 1.1.1.1, 8.8.8.8;
	}
	```

5. Configurar reglas de iptables para NAT de clientes DHCP hacia Internet, mediante la interfaz cableada eth0:

	**sudo iptables -t nat -A POSTROUTING -o eth0 -j MASQUERADE**

	**sudo iptables -A FORWARD -i eth0 -o wlan0 -m state --state RELATED,ESTABLISHED -j ACCEPT**

	**sudo iptables -A FORWARD -i wlan0 -o eth0 -j ACCEPT**
	
	Ejecutar el siguiente comando y guardar reglas IPv4 únicamente:
	
	**sudo apt install iptables-persistent**


6. Configurar archivo de configuración del Access Point:

	**sudo vim /etc/default/hostapd**
	
	```
	DAEMON_CONF="/etc/hostapd/hostapd.conf"
	```

7. Configurar servicio de Access Point:

	**sudo vim /etc/hostapd/hostapd.conf**
	
	```
	ssid=<SSID>
	wpa_passphrase=<PASSWORD>
	interface=wlan0
	auth_algs=3
	channel=7
	driver=nl80211
	hw_mode=g
	logger_stdout=-1
	logger_stdout_level=2
	max_num_sta=5
	rsn_pairwise=CCMP
	wpa=2
	wpa_key_mgmt=WPA-PSK
	wpa_pairwise=TKIP CCMP
	ieee80211n=1
	ieee80211d=1
	wmm_enabled=0
	country_code=US 
	```

