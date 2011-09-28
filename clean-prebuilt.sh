#!/bin/sh

echo 'rm -f share/openzone/ui/cur/*.ozcCur'
rm -f share/openzone/ui/cur/*.ozcCur
echo 'rm -f share/openzone/ui/icon/*.ozcTex'
rm -f share/openzone/ui/icon/*.ozcTex
echo 'rm -f share/openzone/terra/*.{ozTerra,ozcTerra}'
rm -f share/openzone/terra/*.{ozTerra,ozcTerra}
echo 'rm -f share/openzone/sky/*.ozcSky'
rm -f share/openzone/sky/*.ozcSky
echo 'rm -rf share/openzone/bsp/*'
rm -rf share/openzone/bsp/*
echo 'rm -f share/openzone/mdl/*.{ozcSMM,ozcMD2}'
rm -f share/openzone/mdl/*.{ozcSMM,ozcMD2}
