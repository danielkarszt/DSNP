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

?>

<table><tr>

<td id="leftcol">

<div id="details">
<h2><?php print 'Administrator';?></h2>
</div>

</td>

<td id="activity">

<div class="content">

<h1>Create User</h1>

<?php 

echo $form->create( null, array( 'url' => "/admin/snewuser"));

echo $form->input('user');
echo $form->input('pass1', array(
	'label'=> 'password',
	'type' => 'password'
));
echo $form->input('pass2', array(
	'label'=> 'again',
	'type' => 'password'
));

echo $form->end('Create User');

?>
<p>
<?php

#global $CFG_USE_RECAPTCHA;
#
#if ( $CFG_USE_RECAPTCHA ) {
#	require_once('../recaptcha-php-1.10/recaptchalib.php');
#	echo recaptcha_get_html($CFG_RC_PUBLIC_KEY);
#}
?>
<p>

</div>

</td>

</tr></table>
