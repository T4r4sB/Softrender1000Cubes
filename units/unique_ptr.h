namespace std
{
	template <typename T>
	class unique_ptr 
	{
		T* data;
		inline unique_ptr (const unique_ptr& p);
		inline unique_ptr& operator = (const unique_ptr &p);
		inline void clear () {
			typedef char type_must_be_complete[sizeof(T)?1:-1];
			(void) sizeof(type_must_be_complete);
			if(data) {T* tmp=data; data=NULL; delete tmp;} }
	public:
		inline unique_ptr  () : data(NULL) {}
		inline ~unique_ptr ()  { clear(); }
		inline unique_ptr  (T* _data) : data(_data) {}
		inline unique_ptr& operator = (T* _data) { if (data!=_data) { clear(); new(this) unique_ptr(_data); } return *this; }

		inline T& operator *() {return *data;}
		inline operator T* () {return data;}
		inline T* operator -> () {return data; } 
		
		inline const T& operator *() const {return *data;}
		inline operator const T* () const {return data;}
		inline const T* operator -> () const {return data; } 
	};
};