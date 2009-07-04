<?php

/* 
 * Copyright (c) 2009, Adrian Thurston <thurston@complang.org>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

include('../config.php');
include('lib/session.php');

requireOwner();

echo $_FILES['photo']['name'] . "<br>";
echo $_FILES['photo']['tmp_name'] . "<br>";
echo $_FILES['photo']['type'] . "<br>";
echo $_FILES['photo']['size'] . "<br>";
print_r( $_FILES['photo'] );
echo "<br>";

$image_size = @getimagesize( $_FILES['photo']['tmp_name'] );
if ( ! $image_size )
	die( "file doesn't appear to be a valid image" );

# Connect to the database.
$conn = mysql_connect($CFG_DB_HOST, $CFG_DB_USER, $CFG_ADMIN_PASS) or die 
	('Could not connect to database');
mysql_select_db($CFG_DB_DATABASE) or die
	('Could not select database ' . $CFG_DB_DATABASE);

$query = sprintf(
	"INSERT INTO image ( user, rows, cols, mime_type ) " .
	"VALUES( '%s', '%s', '%s', '%s' );",
    mysql_real_escape_string($USER_NAME), 
	$image_size[1], $image_size[0], 
	$image_size['mime']
);

mysql_query( $query ) or die('Query failed: ' . mysql_error());

$result = mysql_query("SELECT last_insert_id() as id") or die('Query failed: ' . mysql_error());
$row = mysql_fetch_assoc($result);
$id = $row['id'];
echo "image id: " . $id;
$path = "$CFG_PHOTO_DIR/$USER_NAME/img-$id.jpg";

if ( ! @move_uploaded_file( $_FILES['photo']['tmp_name'], $path ) )
	die( "bad image file" );

?>
