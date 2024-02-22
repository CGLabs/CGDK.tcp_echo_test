//*****************************************************************************
//*                                                                           *
//*                         CGCII match making system                         *
//*                                                                           *
//*                                test client                                *
//*                                                                           *
//*  Programmed by Cho SangHyun(sangduck@cgcii.co.kr)                         *
//*                                                                           *
//*  Updated : 2021/07/10                                                     *
//*                                                                           *
//*****************************************************************************

#pragma once
// ----------------------------------------------------------------------------
//
// Isocket
//
// ----------------------------------------------------------------------------
class Isocket :
	public						net::socket::tcp<>,
	public						Nmessage_transmitter
{
public:
	object_ptr<Isocket> m_psocket_old;
};

