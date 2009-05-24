<?php

/* 
 * Copyright (c) 2007-2009, Adrian Thurston <thurston@complang.org>
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


function printName( $identity, $possessive )
{
	global $USER_URI;
	global $USER_NAME;
	global $BROWSER_ID;

	if ( !$identity || !isset($BROWSER_ID) && $identity == $USER_URI || 
			isset($BROWSER_ID) && $BROWSER_ID == $identity )
	{
		if ( $possessive )
			echo "your";
		else
			echo "you";
	}
	else if ( isset($BROWSER_ID) && $identity == $USER_URI ) {
		echo $USER_NAME;
		if ( $possessive )
			echo "'s";
	}
	else {
		echo "<a href=\"${identity}\">$identity</a>";
		if ( $possessive )
			echo "'s";
	}
}

function printMessage( $identity, $message, $time_published )
{
	global $USER_NAME;
	global $USER_URI;

	$r = new XMLReader();
	$r->xml( $message );
	if ( $r->read() ) {

		if ( $r->name == "text" ) {
			if ( $r->read() ) {
				$text = $r->value;
			}

			if ( isset( $text ) ) {
				echo "<small>$time_published ";
				printName( $identity, false );
				echo " said:</small><br>";
				echo "&nbsp;&nbsp;" . htmlspecialchars($text) . "<br>";
			}
		}
		else if ( $r->name == "wall" ) {
			if ( $r->read() ) {
				if ( $r->name == "from" ) {
					if ( $r->read() ) {
						$from = $r->value;
						if ( $r->read() && $r->read() ) {
							if ( $r->name == "text" ) {
								if ( $r->read() ) {
									$text = $r->value;
								}
							}
						}
					}
				}
			}

			if ( isset( $from ) && isset( $text ) ) {
				echo "<small>$time_published ";

				printName( $from, false );

				echo " wrote on ";

				printName( $identity, true );

				echo " wall:</small><br>";
				echo "&nbsp;&nbsp;" . htmlspecialchars($text) . "<br>";
			}
		}
	}
}

?>
