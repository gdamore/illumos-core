#! /usr/bin/ksh
#
# This file and its contents are supplied under the terms of the
# Common Development and Distribution License ("CDDL"), version 1.0.
# You may only use this file in accordance with the terms version
# 1.0 of the CDDL.
#
# A full copy of the text of the CDDL should have accompanied this
# source.  A copy is of the CDDL is also available via the Internet
# at http://www.illumos.org/license/CDDL.
#

#
# Copyright 2010 Nexenta Systems, Inc.  All rights reserved.
# Copyright 2014 Garett D'Amore <garrett@damore.org>
#

#
# This extracts all the BSD copyrights (excluding the CDDL licenses)
# for use in a THIRDPARTYLICENSE file.  It tries hard to avoid duplicates.
#

typeset -A LICENSE

function dofile {
	typeset file
	typeset comment
	typeset license
	typeset line
	typeset copyr
	typeset block
	typeset -i i

	typeset nl=$(print)
	file=$1 ; shift

	comment=
	unset license
	unset block
	copyr=

	cat $file | while IFS="" read line; do
		if [[ "$line" == /* ]] ; then
			comment=y
			copyr=
			block=
			continue
		fi

		if [[ -z $comment ]]; then
			# not in a comment
			continue
		fi

		#
		# We don't want to know about CDDL files.  They don't
		# require an explicit THIRDPARTYLICENSE file.
		#
		if [[ "$line" == *CDDL* ]]
		then
			return
		fi

		if [[ "$line" == *Copyright* ]]
		then
			copyr=y
		fi

		if [[ "$line" != */ ]]
		then
			line="${line# \* }"
			line="${line# \*}"
			line="${line% \*/}"
			# append to block array
			block="${block}${line}"'\n'
			continue
		fi

		#
		# We have reached the end of the comment now.
		#
		comment=

		# Check to see if we saw a copyright.
		if [[ -z "$copyr" ]]; then
			block=
			continue
		fi
		license="${license}${block}"
		block=
	done

	if [[ -n "$license" ]]
	then
		LICENSE["${license}"]="${LICENSE["${license}"]} $file"
	fi
}

for a in $*
do
	find "$a" -type f -name '*.[chs]' -print | while read f
	do
		dofile $f
	done
done

for lic in "${!LICENSE[@]}"; do
	print "The following files from the C library:"
	for f in ${LICENSE[$lic]}; do
		print "    $f"
	done
	print "are provided under the following terms:"
	print
	print "$lic"
done
