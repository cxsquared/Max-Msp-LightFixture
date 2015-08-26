#!/bin/bash
echo "Building!"
ruby ../../build.rb
echo "Copying files"
mkdir bin
cp -r ../../../sdk-build/externals/lightfixture.mxo bin/
cp lightfixture.maxhelp bin/
echo "Building done"
