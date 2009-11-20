<div id="leftcol">

<div id="details">
<h2><?php echo $USER['display_short'];?></h2>
</div>

</div>

<div id="activity">

<div class="content">
<h3>Wish List for <?echo $this->data['FriendClaim']['name']; ?></h3>

<pre>
<?php 
if ( isset( $this->data['Wish'] ) )
	echo htmlspecialchars($this->data['Wish']['list']);
else
	echo "<em>This user has not yet given a wishlist</em>";
?>

</pre>

<?php
$id = $this->data['FriendClaim']['id'];
if ( $id == $BROWSER_FC['id'] )
	echo $html->link( 'edit', "/$USER_NAME/wish/edit/$id" );
?>

</div>
</div>