#!/bin/bash

# Assign the argument to a variable
VIRTUAL_INTERFACE_NAME="vapInterf"

# Replace 'vapInterf' with the variable
sudo iw dev wlan0 interface add $VIRTUAL_INTERFACE_NAME type __ap
sudo ip addr add 192.168.4.1/24 dev $VIRTUAL_INTERFACE_NAME

sudo ip link set wlan0 up
sudo ip link set $VIRTUAL_INTERFACE_NAME up

sudo systemctl stop hostapd
sudo systemctl stop dnsmasq

sudo hostapd /etc/hostapd/hostapd-vapInterf.conf
sudo dnsmasq -C /etc/dnsmasq.conf