#pragma once

enum FSMStatus
{
	FSM_EMPTY = 1,
	FSM_DOING,
	FSM_DONE
};

enum BLOCKStatus
{
	BLOCK_EMPTY = 1,
	BLOCK_RECVING,
	BLOCK_RECVED
};
