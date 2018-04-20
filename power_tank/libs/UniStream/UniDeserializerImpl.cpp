//#include "UniDeserializerImpl.h"
//#include "UniTypeTools.h"
//namespace uni
//{
//    namespace deserializer_impl
//    {
//        /////////////////////////////////////////////////////////////////////
//        /////////////////////////////////////////////////////////////////////

//        /////////////////////////////////////////////////////////////////////
//        template<typename RemoteType, typename LocalType>
//        inline void CastCopy(LocalType* dst, const uint8_t* buff) noexcept
//        {
//            RemoteType remoteVal;
//            CopyFromBuff<sizeof(remoteVal)>(&remoteVal, buff);
//            *dst = static_cast<LocalType>(remoteVal);
//        }

//        /////////////////////////////////////////////////////////////////////
//        template<typename RemoteType, typename LocalType>
//        inline void CastArrayCopy(LocalType* dst, const uint8_t* buff, size_t count) noexcept
//        {
//            RemoteType remoteVal;
//            LocalType* arrayEnd = dst + count;
//            while(dst < arrayEnd)
//            {
//                CopyFromBuff<sizeof(remoteVal)>(&remoteVal, buff);
//                *dst = static_cast<LocalType>(remoteVal);
//                dst++;
//                buff += sizeof (remoteVal);
//            }
//        }

//        /////////////////////////////////////////////////////////////////////
//        template<typename T>
//        void DeserializeNonArrayFundamental(T* val, uint8_t remoteCode, const uint8_t* buff) noexcept
//        {
//            using namespace types;
//            if (false == types::Checker<T>::CastNeeded(remoteCode))
//            {
//                CopyFromBuff<sizeof(T)>(val, buff);
//                return;
//            }

//            switch(remoteCode)
//            {
//                // signed 8
//                case (_markFundamental | _markIntegral | _markSigned | _markTypeSize_1):
//                {
//                    CastCopy<int8_t, T>(val, buff);
//                }
//                break;
//                // signed 16
//                case (_markFundamental | _markIntegral | _markSigned | _markTypeSize_2):
//                {
//                    CastCopy<int16_t, T>(val, buff);
//                }
//                break;
//                // signed 32
//                case (_markFundamental | _markIntegral | _markSigned | _markTypeSize_4):
//                {
//                    CastCopy<int32_t, T>(val, buff);
//                }
//                break;
//                // signed 64
//                case (_markFundamental | _markIntegral | _markSigned | _markTypeSize_8):
//                {
//                    CastCopy<int64_t, T>(val, buff);
//                }
//                break;
//                // unsigned 8
//                case (_markFundamental | _markIntegral | _markUnsigned | _markTypeSize_1):
//                {
//                    CastCopy<uint8_t, T>(val, buff);
//                }
//                break;
//                // unsigned 16
//                case (_markFundamental | _markIntegral | _markUnsigned | _markTypeSize_2):
//                {
//                    CastCopy<uint16_t, T>(val, buff);
//                }
//                break;
//                // unsigned 32
//                case (_markFundamental | _markIntegral | _markUnsigned | _markTypeSize_4):
//                {
//                    CastCopy<uint32_t, T>(val, buff);
//                }
//                break;
//                // unsigned 64
//                case (_markFundamental | _markIntegral | _markUnsigned | _markTypeSize_8):
//                {
//                    CastCopy<uint64_t, T>(val, buff);
//                }
//                break;
//                // float 32
//                case (_markFundamental | _markNonIntegral | _markSigned | _markTypeSize_4):
//                {
//                    CastCopy<float, T>(val, buff);
//                }
//                break;
//                // double 64
//                case (_markFundamental | _markNonIntegral | _markSigned | _markTypeSize_8):
//                {
//                    CastCopy<double, T>(val, buff);
//                }
//                break;
//            }
//        }

//        /////////////////////////////////////////////////////////////////////
//        template<typename T>
//        void DeserializeArrayFundamental(T* val, uint8_t remoteCode, const uint8_t* buff, size_t count) noexcept
//        {
//            using namespace types;
//            if (false == types::Checker<T>::CastNeeded(remoteCode))
//            {
//                T* arrayEnd = val + count;
//                while(val < arrayEnd)
//                {
//                    CopyFromBuff<sizeof(T)>(val, buff);
//                    val++;
//                    buff += sizeof (T);
//                }
//                return;
//            }

//            // remove array flag
//            remoteCode &= (~_arrayMask);

//            switch(remoteCode)
//            {
//                // signed 8
//                case (_markFundamental | _markIntegral | _markSigned | _markTypeSize_1):
//                {
//                    CastArrayCopy<int8_t, T>(val, buff, count);
//                }
//                break;
//                // signed 16
//                case (_markFundamental | _markIntegral | _markSigned | _markTypeSize_2):
//                {
//                    CastArrayCopy<int16_t, T>(val, buff, count);
//                }
//                break;
//                // signed 32
//                case (_markFundamental | _markIntegral | _markSigned | _markTypeSize_4):
//                {
//                    CastArrayCopy<int32_t, T>(val, buff, count);
//                }
//                break;
//                // signed 64
//                case (_markFundamental | _markIntegral | _markSigned | _markTypeSize_8):
//                {
//                    CastArrayCopy<int64_t, T>(val, buff, count);
//                }
//                break;
//                // unsigned 8
//                case (_markFundamental | _markIntegral | _markUnsigned | _markTypeSize_1):
//                {
//                    CastArrayCopy<uint8_t, T>(val, buff, count);
//                }
//                break;
//                // unsigned 16
//                case (_markFundamental | _markIntegral | _markUnsigned | _markTypeSize_2):
//                {
//                    CastArrayCopy<uint16_t, T>(val, buff, count);
//                }
//                break;
//                // unsigned 32
//                case (_markFundamental | _markIntegral | _markUnsigned | _markTypeSize_4):
//                {
//                    CastArrayCopy<uint32_t, T>(val, buff, count);
//                }
//                break;
//                // unsigned 64
//                case (_markFundamental | _markIntegral | _markUnsigned | _markTypeSize_8):
//                {
//                    CastArrayCopy<uint64_t, T>(val, buff, count);
//                }
//                break;
//                // float 32
//                case (_markFundamental | _markNonIntegral | _markSigned | _markTypeSize_4):
//                {
//                    CastArrayCopy<float, T>(val, buff, count);
//                }
//                break;
//                // double 64
//                case (_markFundamental | _markNonIntegral | _markSigned | _markTypeSize_8):
//                {
//                    CastArrayCopy<double, T>(val, buff, count);
//                }
//                break;
//            }
//        }

//        void DeserializeArrayHeader(const uint8_t* buff, uint8_t& remoteCode, size_t& size)
//        {
//            remoteCode = *buff++;
//            uint32_t sz;
//            CopyFromBuff<4>(&sz, buff);
//            size = static_cast<size_t>(sz);
//        }

//        /////////////////////////////////////////////////////////////////////
//        /////////////////////////////////////////////////////////////////////
//        /////////////////////////////////////////////////////////////////////

//        UNISTR_DECLARE_ALL_FUNDAMENTALS(void, DeserializeNonArrayFundamental, ,*, uint8_t, const uint8_t*)
//        UNISTR_DECLARE_ALL_FUNDAMENTALS(void, DeserializeArrayFundamental, ,*, uint8_t, const uint8_t*, size_t)


//    }
//}
