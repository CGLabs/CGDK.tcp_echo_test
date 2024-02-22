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
// message heaser
struct PACKET_HEADER_BASE
{
	short size_ = 0;
	short type = 0;

	operator size_t() const noexcept { return (size_t)size_; }
	static void			_set_message_size(buffer& _buffer) noexcept { _buffer.front<short>(0) = _buffer.size<short>(); }
	static std::size_t	_get_message_size(const buffer_view& _buffer) noexcept { return static_cast<std::size_t>(*_buffer.data<short>(0)); }
	static bool			_validate_message(const std::vector<shared_buffer>& /*_vector_buffer*/) noexcept { return true; }
};

class Isocket :
	public						net::socket::tcp<PACKET_HEADER_BASE>,
	public						Nmessage_transmitter
{
public:
	object_ptr<Isocket> m_psocket_old;
};

