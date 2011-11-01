#!/bin/sh

echo 'rm -f share/openzone/ui/cur/*.ozcCur'
rm -f share/openzone/ui/cur/*.ozcCur
echo 'rm -f share/openzone/ui/icon/*.ozcTex'
rm -f share/openzone/ui/icon/*.ozcTex
echo 'rm -f share/openzone/terra/*.{ozTerra,ozcTerra}'
rm -f share/openzone/terra/*.{ozTerra,ozcTerra}
echo 'rm -f share/openzone/sky/*.ozcSky'
rm -f share/openzone/sky/*.ozcSky
echo 'rm -f share/openzone/data/maps/*{autosave.map,.bak}'
rm -f share/openzone/data/maps/*{autosave.map,.bak}
echo 'rm -f share/openzone/data/maps/*.{bsp,prt,srf}'
rm -f share/openzone/data/maps/*.{bsp,prt,srf}
echo 'rm -f share/openzone/bsp/*.{ozBSP,ozcBSP}'
rm -f share/openzone/bsp/*.{ozBSP,ozcBSP}
echo 'rm -f share/openzone/bsp/*/*.ozcTex'
rm -f share/openzone/bsp/*/*.ozcTex
echo 'rm -f share/openzone/mdl/*.{ozcSMM,ozcMD2}'
rm -f share/openzone/mdl/*.{ozcSMM,ozcMD2}
echo 'rm -f share/openzone/ui/galileo/*.ozcTex'
rm -f share/openzone/ui/galileo/*.ozcTex
