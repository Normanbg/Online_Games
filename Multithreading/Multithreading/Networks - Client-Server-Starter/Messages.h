#pragma once

// Add as many messages as you need depending on the
// functionalities that you decide to implement.

enum class ClientMessage
{
	Hello,
	Send
};

enum class ServerMessage
{
	Welcome,
	UserNameExists,
	NewUser
};

