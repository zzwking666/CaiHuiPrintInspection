#ifndef OSO_CORE_H_
#define OSO_CORE_H_

#include<vector>
#include<memory>
#include<string>
#include<iostream>

namespace rw {
	namespace oso {
		class ObjectStoreCore {
		protected:
			/**
			 * @brief The name of the object store
			 *
			 *  This is a private member of class ObjectStoreCore.Means the name representation of the instance object, which can serve as the node name in the class composition structure.
			 */
			std::string m_name;

		public:
			/**
			 *@Parameters:
			 *  - name: The name you are going to set as
			 *@Methods:
			 *  Set the name of the object store
			 *@Returns: void
			 *
			 *@Throws:
			 *
			 */
			void setName(const std::string& name);

			/**
			 *@Parameters:
			 *  void
			 *@Methods:
			 *  Get the name of the object store
			 *@Returns: std::string
			 *  The name of the object store
			 *@Throws:
			 *
			 */
			std::string getName() const;

		public:
			/**
			 * @brief The parent object of the object store
			 *
			 * Point to the parent object, if any
			 */
			ObjectStoreCore* parent{ nullptr };

		public:
			/**
			 * @brief The level of the object store
			 *
			 * This is a private member of class ObjectStoreCore.Means the level of the instance object, which can serve as the node level in the class composition structure.
			 *
			 * @Supplement:
			 *
			 * Hierarchical structure:He represents the level of the instance in the object tree, for example, if he has a parent object and the parent object has no parent bject, then his level is 1
			 */
			size_t level{ 0 };

			/**
			 *@Parameters:
			 * void
			 *@Methods:
			 * Increase the hierarchy of class instances
			 *@Returns: void
			 *
			 *@Throws:
			 *
			 */
			virtual void addLevel() {
				level++;
			}

			/**
			 *@Parameters:
			 *  void
			 *@Methods:
			 *  Decrease the
			 *@Returns: types hierarchy of class instances
			 *
			 *@Throws:
			 *
			 */
			virtual void subLevel() {
				if (level == 0) {
					return;
				}
				level--;
			}

		public:

			/**
			 *@Parameters:
			 *  void
			 *@Methods:
			 *  Get the type of the object store
			 *@Returns: std::string
			 *  The type of the object store
			 *@Throws:
			 *
			 */
			virtual std::string getStoreType() const {
				return "core";
			}

		public:
			ObjectStoreCore() = default;

			ObjectStoreCore(const ObjectStoreCore& core);

			ObjectStoreCore(ObjectStoreCore&& core) noexcept;

			ObjectStoreCore(std::vector<char> binary);

		public:
			ObjectStoreCore& operator=(ObjectStoreCore&& other) noexcept;

			virtual ~ObjectStoreCore() = default;

		public:

			/**
			 *@Parameters:
			 *  - os: Output stream
			 *@Methods:
			 *  Print the object tree of the Object Store with the current instance as the root node
			 *@Returns: void
			 *
			 *@Throws:
			 *
			 */
			virtual void print(std::ostream& os);
		public:
			ObjectStoreCore& operator=(const ObjectStoreCore& other);
		public:
			virtual bool operator==(const ObjectStoreCore& other) const;

			virtual bool operator!=(const ObjectStoreCore& other) const;

		public:
			//virtual std::vector<char> getBinary();
		};

		//The type of data stored in the Object Data Item class
		enum  class ObjectDataItemStoreType
		{
			item_string,
			item_int,
			item_long,
			item_float,
			item_double,
			item_bool
		};

		//Composite object, which can combine classes based on Object Store Core
		class ObjectStoreItem
			:public ObjectStoreCore {
		private:
			//The value of the object data item
			std::string _value{ "0" };

			//The type of data stored in the object data item
			ObjectDataItemStoreType _type{ ObjectDataItemStoreType::item_int };
		public:
			ObjectStoreItem() = default;

			ObjectStoreItem(const ObjectStoreItem& item);

			ObjectStoreItem(ObjectStoreItem&& item) noexcept;

			ObjectStoreItem(std::vector<char> binary);

			~ObjectStoreItem() override = default;
		public:
			ObjectStoreItem& operator=(const ObjectStoreItem& other);

			ObjectStoreItem& operator=(ObjectStoreItem&& other) noexcept;

		public:
			//Parameters:
			//      value: The value you are going to set as
			void setValueFromString(const std::string& value);

			//Parameters:
			//      value: The value you are going to set as
			void setValueFromInt(int value);

			//Parameters:
			//      value: The value you are going to set as
			void setValueFromLong(long value);

			//Parameters:
			//      value: The value you are going to set as
			void setValueFromFloat(float value);

			//Parameters:
			//      value: The value you are going to set as
			void setValueFromDouble(double value);

			//Parameters:
			//      value: The value you are going to set as
			void setValueFromBool(bool value);

		public:
			//getValueAsString::Get the value of the object data item as a string
			std::string getValueAsString() const;

			//getValueAsInt::Get the value of the object data item as an integer
			int getValueAsInt() const;

			//getValueAsLong::Get the value of the object data item as a long integer
			long getValueAsLong() const;

			//getValueAsFloat::Get the value of the object data item as a floating point number
			float getValueAsFloat() const;

			//getValueAsDouble::Get the value of the object data item as a double precision floating point number
			double getValueAsDouble() const;

			//getValueAsBool::Get the value of the object data item as a boolean value
			bool getValueAsBool() const;

			//getType::Get the type of data stored in the object data item
			ObjectDataItemStoreType getType() const;

			//getTypeString::Get the type of data stored in the object data item as a string
			std::string getTypeString() const;

		public:
			// Inheriting through Object Store Core

			//Parameters:
			//      os: Output stream
			//Print the object tree of the Object Store with the current instance as the root node
			void print(std::ostream& os) override;

			//Get the type of the object store
			virtual std::string getStoreType() const override {
				return "item";
			}

		public:
			bool equlas(const ObjectStoreItem& other) const;

			bool operator==(const ObjectStoreCore& other) const override;

			bool operator!=(const ObjectStoreCore& other) const override;

			bool operator==(const ObjectStoreItem& other) const;

			bool operator!=(const ObjectStoreItem& other)const;

		public:
			//std::vector<char> getBinary()override;
		};

		//
		class ObjectStoreAssembly
			:public ObjectStoreCore {
		private:
			//Store sub objects, which can be Object Store Assembly and Object Store Item
			std::vector<std::shared_ptr<ObjectStoreCore>> _items;

		public:
			ObjectStoreAssembly() = default;

			ObjectStoreAssembly(const ObjectStoreAssembly& assembly);

			ObjectStoreAssembly(ObjectStoreAssembly&& assembly) noexcept;

			ObjectStoreAssembly(std::vector<char> binary);

			virtual ~ObjectStoreAssembly() = default;

		public:
			ObjectStoreAssembly& operator=(const ObjectStoreAssembly& other);

			ObjectStoreAssembly& operator=(ObjectStoreAssembly&& other) noexcept;

		public:
			//Parameters:
			//      item: The item you are going to add
			//
			//addItem: Add an item to the object store
			void addItem(std::shared_ptr<ObjectStoreCore> item);

			//Parameters:
			//      assembly: The assembly you are going to add
			//
			//addItem: Add an assembly to the object store
			void addItem(ObjectStoreAssembly&& assembly);

			void addItem(const ObjectStoreAssembly& item);

			//Parameters:
			//      item: The item you are going to add
			//
			//addItem: Add an item to the object store
			void addItem(ObjectStoreItem&& item);

			void addItem(const ObjectStoreItem& item);

			//Parameters:
			//      name: The name of the item you are going to remove
			//
			//removeItem: Remove an item from the object store
			void removeItem(std::shared_ptr<ObjectStoreCore> item);

		public:
			using size_type = std::vector<std::shared_ptr<ObjectStoreCore>>::size_type;

			//Parameters:
			//      i: The index of the item you are going to get
			//
			//operator[]: Get the item by index
			std::shared_ptr<ObjectStoreCore> operator[](const size_type& i);

			//Parameters:
			//      i: The index of the item you are going to get
			//
			//at: Get the item by index
			const std::shared_ptr<ObjectStoreCore> at(const size_type& i)const;

		public:
			//Get the number of items in the object store
			std::vector<std::shared_ptr<ObjectStoreCore>> getItems() const;

			//Parameters:
			//      name: The name of the item you are going to get
			//
			//getItem: Get the item by name if it exists, otherwise return nullptr
			std::shared_ptr<ObjectStoreCore> getItem(const std::string& name) const;

		public:
			//Parameters:
			//      os: Output stream
			// print:Inheriting through Object Store Core
			void print(std::ostream& os) override;

			virtual void addLevel() override;

			virtual void subLevel() override;

			virtual std::string getStoreType() const override {
				return "assembly";
			}
		public:
			bool equlas(const ObjectStoreAssembly& other) const;

			bool operator==(const ObjectStoreCore& other) const override;

			bool operator!=(const ObjectStoreCore& other)const override;

			bool operator==(const ObjectStoreAssembly& other) const;

			bool operator!=(const ObjectStoreAssembly& other)const;

		public:
			//std::vector<char> getBinary() override;
		};

		inline std::shared_ptr<ObjectStoreAssembly>
			makeObjectStoreAssemblySharedPtr
			(const ObjectStoreAssembly& assembly) {
			return std::make_shared<ObjectStoreAssembly>(assembly);
		}

		inline std::shared_ptr<ObjectStoreItem>
			makeObjectStoreItemSharedPtr
			(const ObjectStoreItem& item) {
			return std::make_shared<ObjectStoreItem>(item);
		}

		inline std::shared_ptr<ObjectStoreAssembly>
			ObjectStoreCoreToAssembly
			(std::shared_ptr<ObjectStoreCore> core) {
			std::shared_ptr<ObjectStoreAssembly> assembly = std::dynamic_pointer_cast<ObjectStoreAssembly>(core);

			return assembly;
		}

		inline std::shared_ptr<ObjectStoreItem>
			ObjectStoreCoreToItem
			(std::shared_ptr<ObjectStoreCore> core) {
			std::shared_ptr<ObjectStoreItem> item = std::dynamic_pointer_cast<ObjectStoreItem>(core);

			return item;
		}
	}
}

#endif // !OSO_CORE_H_
