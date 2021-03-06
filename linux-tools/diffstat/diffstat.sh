#!/bin/bash
###########################################################################################
## Copyright 2003, 2015 IBM Corp                                                          ##
##                                                                                        ##
## Redistribution and use in source and binary forms, with or without modification,       ##
## are permitted provided that the following conditions are met:                          ##
##	1.Redistributions of source code must retain the above copyright notice,          ##
##        this list of conditions and the following disclaimer.                           ##
##	2.Redistributions in binary form must reproduce the above copyright notice, this  ##
##        list of conditions and the following disclaimer in the documentation and/or     ##
##        other materials provided with the distribution.                                 ##
##                                                                                        ##
## THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS AND ANY EXPRESS       ##
## OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF        ##
## MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ##
## THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,    ##
## EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF     ##
## SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) ##
## HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,  ##
## OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS  ##
## SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                           ##
############################################################################################
### File :        diffstat.sh                                                  ##
##
### Description:  Diffstat is commonly used                                    ##
##
### Author:       Anitha MallojiRao amalloji@in.ibm.com                        ##
###########################################################################################
## source the utility functions

#cd $(dirname $0)
#LTPBIN=${PWD%%/testcases/*}/testcases/bin
source $LTPBIN/tc_utils.source
DIFF_TEST_DIR="${LTPBIN%/shared}/diffstat"
REQUIRED="diffstat"

function tc_local_setup()
{
    # Check installation
    tc_exec_or_break $REQUIRED 
    pushd $DIFF_TEST_DIR/testing  &> /dev/null
    TESTS=`ls *.pat`
    TST_TOTAL=`echo $TESTS | wc -w`
    if [ -f run_test.sh ]; then
        tc_break_if_bad $? "There is no run_test.sh script to run the tests. Check if diffstat-fivextra is installed properly"
    fi
    popd &> /dev/null
}

function run_tests()
{
    pushd $DIFF_TEST_DIR  &> /dev/null
    for i in $TESTS;
    do
       tc_register "$i"
       ./testing/run_test.sh ./testing/$i> $stdout 2>$stderr
       grep fail $stdout
       [ $? -ne 0 ]
       tc_pass_or_fail $? "$i test failed failed"
    done
    popd &> /dev/null 
}

#
# main
#
tc_setup
run_tests
