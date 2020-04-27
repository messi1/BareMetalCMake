#pragma once

  #include <algorithm>
  #include <cstddef>
  #include <cstdint>

  namespace util
  {
    class communication_base
    {
    public:
      using size_type=std::size_t;

      virtual ~communication_base()=default;

      virtual bool send(const std::uint8_t byte_to_send) = 0;
      virtual bool recv(std::uint8_t& byte_to_recv) = 0;
      virtual size_type recv_ready() const = 0;
      virtual bool idle() const = 0;

      template<typename send_iterator_type>
      bool send_n(send_iterator_type first,
                  send_iterator_type last)
      {
        bool send_result = true;

        while(first != last)
        {
          typedef typename
          std::iterator_traits<send_iterator_type>::value_type
          send_value_type;

          const send_value_type value(*first);

          send_result &= send(std::uint8_t(value));

          ++first;
        }

        return send_result;
      }

      template<typename recv_iterator_type>
      bool recv_n(recv_iterator_type first,
                  size_type count)
      {
        const size_type count_to_recv = (std::min)(count, recv_ready());

        recv_iterator_type last = first + count_to_recv;

        bool recv_result = true;

        while(first != last)
        {
          std::uint8_t byte_to_recv;

          recv_result &= recv(byte_to_recv);

          typedef typename
          std::iterator_traits<recv_iterator_type>::value_type
          recv_value_type;

          *first = recv_value_type(byte_to_recv);

          ++first;
        }

        return recv_result;
      }

      template<typename recv_iterator_type>
      bool recv_n(recv_iterator_type first, recv_iterator_type last)
      {
        const size_type count_to_recv = size_type(std::distance(first, last));

        return recv_n(first, count_to_recv);
      }

    protected:
      communication_base() noexcept = default;
    };

    class communication_buffer_depth_one_byte : public communication_base
    {
    public:
      using buffer_type=std::uint8_t;
      
      virtual ~communication_buffer_depth_one_byte() = default;

      virtual bool recv(std::uint8_t& byte_to_recv)
      {
        const bool recv_is_ok = recv_buffer_is_full;

        if(recv_is_ok)
        {
          byte_to_recv = recv_buffer;

          recv_buffer_is_full = false;
        }

        return recv_is_ok;
      }

      virtual size_type recv_ready() const
      {
        return (recv_buffer_is_full ? 1U : 0U);
      }

      virtual bool idle() const
      {
        return true;
      }

    protected:
      communication_buffer_depth_one_byte() noexcept = default;

      buffer_type recv_buffer{};
      bool recv_buffer_is_full{false};
    };
  }

#endif // UTIL_COMMUNICATION_2012_05_31_H_
