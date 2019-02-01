# MQTT

## Descripción

El script de python ``ttn2db.py`` se encarga de leer desde TTN paquetes de MQTT, e insertarlos en una base de datos de TinyDB. Se recomienda ejecutar en conjunto con SystemD como un servicio, y es capaz de escuchar varios sensores de forma simultánea, agregando sus datos en la base de datos según el tipo de sensor.


## Instrucciones de instalación

```
python3 -m "venv" venv
venv/bin/pip3 install -r requirements.txt
venv/bin/python3 ttn2db.py
```

## Configuración de servicio a nivel de Systemd

1. Crear script de systemd:

    vim /etc/systemd/system/ttn2db.service

    ```
    [Unit]
    Description= loads data from TTN and save it in a local tinydb file in json format
    After=syslog.target

    [Service]
    Type=simple
    User=root
    Group=root
    Environment="PYTHONUNBUFFERED=1"
    ExecStart=/opt/venv/bin/python3 /opt/MQTT/ttn2db.py
    StandardOutput=syslog
    StandardError=syslog
    SyslogIdentifier=ttn2db
    Restart=always

    [Install]
    WantedBy=multi-user.target
    ```

2. Aplicar los cambios:

    ```
    sudo systemctl daemon-reload
    sudo systemctl enable ttn2db
    sudo systemctl start ttn2db
    ```

3. Para crear un archivo de log personalizado:

    - Crear el archivo /etc/rsyslog.d/ttn2db.conf:

        ```
        if $programname == 'ttn2db' then /var/log/ttn2db.log

        ```
    - Reiniciar el servicio para aplicar cambios:
        ``` 
        sudo systemctl restart rsyslog.service
        ```

    - Confirmar el correcto funcionamiento visualizando el log:
        sudo tail -f /var/log/ttn2db.log

