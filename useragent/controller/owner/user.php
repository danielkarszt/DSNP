<?php
class OwnerUserController extends Controller
{
	var $function = array(
		'broadcast' => array(
			array(
				post => 'message'
			),
		),
		'flush' => array(
			array( 
				get => 'reqid', 
				type => 'base64', 
				length => TOKEN_BASE64_SIZE
			),
			array(
				get => 'backto'
			)
		),
		'edit' => array(),
		'sedit' => array(
			array( post => 'id' ),
			array( post => 'name' ),
			array( post => 'email' ),
		),
	);

	function broadcast()
	{
		$text = trim( $this->args['message'] );

		dbQuery( "
			INSERT INTO activity ( user_id, published, type, message )
			VALUES ( %e, true, 'MSG', %e )", $this->USER[ID], $text );

		$message = new Message;
		$message->broadcast( $text );

		$connection = new Connection;
		$connection->openLocalPriv();
		$connection->submitBroadcast( 
			$this->USER[USER], '-', $message->message );

		if ( $connection->success )
			$this->userRedirect( "/" );
		else
			die( "submit_broadcast failed with $connection->result" );
	}

	function flush()
	{
		$reqid = $this->args['reqid'];
		$backto = $this->args['backto'];

		$connection = new Connection;
		$connection->openLocalPriv();

		$connection->remoteBroadcastResponse(
			$this->USER[USER], $reqid );

		if ( !$connection->success )
			die( "remote_broadcast_response failed with $connection->result");
		$reqid = $connection->regs[1];

		$this->redirect( "${backto}user/finish?reqid=$reqid" );
	}

	function edit()
	{
		$user = dbQuery( 
			"SELECT * FROM user WHERE user = %e", 
			$this->USER[USER] );
		$this->vars['user'] = $user;
	}

	function sedit()
	{
		$id = $this->args['id'];
		$name = $this->args['name'];
		$email = $this->args['email'];

		if ( preg_match( '/^[ \t\n]*$/', $name ) )
			$name = null;
		if ( preg_match( '/^[ \t\n]*$/', $email ) )
			$email = null;

		if ( $id === $this->USER[ID] ) {
			dbQuery( 
				"UPDATE user SET name = %e, email = %e WHERE id = %l",
				$name, $email, $this->USER[ID] );
		}

		/* User Message */
		$message = new Message;
		$message->nameChange( $name );

		$connection = new Connection;
		$connection->openLocalPriv();
		$connection->submitBroadcast( $this->USER[USER],
				'-', $message->message );

		if ( $connection->success )
			$this->userRedirect( "/" );
		else
			die( "submit_broadcast failed with $connection->result" );
	}
}
?>