<html xmlns="http://www.w3.org/1999/xhtml">
<head>
	<?php /*echo $html->charset(); */?>
	<title>
		<?php 
			echo $CFG[SITE_NAME];
			echo ": {$USER['USER']}";
		?>
	</title>
	<link href="<?echo $this->siteLoc( '/css/dsnpua.css' ); ?>"
		rel="stylesheet" type="text/css"/>
</head>
<body>
	<div id="header">
	<table><tr>
		<td id="header_left"><div class="header_content">
		<h1>
				<?php echo $this->siteLink( $CFG['SITE_NAME'], '/' ); ?>
		</h1>
		</div></td>

		<td id="header_middle"><div class="header_content">
		<h1>
		<?php
			echo $this->userLink( $USER['USER'], '/' );
		?>
		</h1>
		</div></td>

		<td id="header_right"><div class="header_content">
		<h1>
		<?php 
			echo $this->userLink( 'login', '/cred/login' );
		?>
		</h1>
		</div></td>

	</tr></table>
	</div>

	<div id="page_body">
