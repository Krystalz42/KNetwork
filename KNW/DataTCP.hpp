#ifndef NETWORK_DATATCP_HPP
#define NETWORK_DATATCP_HPP

#include <BaseDataType.hpp>
#include <functional>
#include <vector>

namespace KNW {

	class DataTCP {

	public:
		using Header = BaseDataType::Header;

		DataTCP();
		DataTCP(DataTCP const &) = delete;

		template<typename T>
		void addDataType(std::function<void(T)> callback);

		template<typename T>
		bool hasType() const;

		void sendDataToCallback(Header header, void *data);
		size_t getSizeOfHeader(Header header);

		~DataTCP();

	private:


		class AbstractCallback {
		public:
			virtual ~AbstractCallback() = default;

			virtual void operator()(void *) = 0;
		};

		template<typename T>
		class CallbackType : public AbstractCallback {
		public:
			explicit CallbackType(std::function<void(T)> function)
					: function_(function) {}

			void operator()(void *pVoid) override {
				T data;
				std::memcpy(static_cast<void *>(&data), pVoid, sizeof(T));
				return function_(data);
			}

			std::function<void(T)> function_;
		};

		template<typename T>
		size_t getSizeOfType();

		template<typename T>
		Header getHeaderOfType();

		friend class ServerTCP;
		friend class ConnectionTCP;
		friend class ClientTCP;

		std::vector<size_t> sizeType;
		std::vector<std::shared_ptr<AbstractCallback>> callbackType;
	};


	template<typename T>
	void DataTCP::addDataType(std::function<void(T)> callback) {
		auto header = DataType<T>::getHeader();
		assert(!hasType<T>());

		sizeType.resize(header + 1);
		callbackType.resize(header + 1);

		sizeType[header] = sizeof(T);
		callbackType[header] = std::make_shared<CallbackType<T>>(callback);

	}

	template<typename T>
	bool DataTCP::hasType() const {
		auto header = DataType<T>::getHeader();
		return header < sizeType.size() && header < callbackType.size();
	}

	template<typename T>
	size_t DataTCP::getSizeOfType() {
		assert(hasType<T>());
		return sizeType[DataType<T>::getHeader()];
	}

	template<typename T>
	DataTCP::Header DataTCP::getHeaderOfType() {
		assert(hasType<T>());
		return DataType<T>::getHeader();
	}

}
#endif //NETWORK_DATATCP_HPP
