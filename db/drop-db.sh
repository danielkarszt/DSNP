#!/bin/bash
#

runfrom=`dirname $0`
. $runfrom/common.sh

mysql_cmd mysql << EOF
	DROP DATABASE ${site_name};
	DROP USER '${site_name}_dsnp'@'localhost';
	DROP USER '${site_name}_ua'@'localhost';
EOF

