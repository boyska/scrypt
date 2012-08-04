#!/usr/bin/env zsh

make base64_test || exit 5
error=0
while read line; do
	if [[ $line =~ '^ *$' ]] || [[ $line =~ '^#' ]]; then
		continue
	fi
	test_enc=`cut -f1 <<<$line`
	test_dec=`cut -f2- <<<$line`
	dec_path=`mktemp /tmp/base64_test.dec.XXX`
	enc_path=`mktemp /tmp/base64_test.enc.XXX`
	./base64_test $test_enc > $dec_path 2> $enc_path
	if [ "$test_dec" != "`cat $dec_path`" ]; then
		echo "Wrong testcase(encoding): $test_enc => `cat $dec_path`, expected $test_dec" >&2
		error=$((error+1))
	fi
	if [ "$test_enc" != "`cat $enc_path`" ]; then
		echo "Wrong testcase(reencoding): $test_dec => `cat $enc_path`, expected $test_enc" >&2
		error=$((error+1))
	fi

done < test_base64.txt

if [ "$error" -gt 0 ]; then
	exit $error #TODO: min($error, 127)
fi
