// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
 
#pragma once
 
#include <memory>
 
namespace carla {
 
    /// AtomicSharedPtr��һ���ǳ��򵥵�ԭ�Ӳ�������ָ���֧࣬���̰߳�ȫ�Ĺ���ָ�������ʹ����release-acquire�ڴ�˳��
  template <typename T>
  class AtomicSharedPtr {
  public:
  	
 	// ���캯����ʹ��ת���������������������ת����shared_ptr���г�ʼ��
    template <typename... Args>
    explicit AtomicSharedPtr(Args &&... args)
      : _ptr(std::forward<Args>(args)...) {}
      
 	// �������캯����ʹ��load()��������ȡ��ǰ����ָ���ֵ����ʼ��
    AtomicSharedPtr(const AtomicSharedPtr &rhs)
      : _ptr(rhs.load()) {}
      
 	 // ɾ���ƶ����캯���������������ƶ�����
    AtomicSharedPtr(AtomicSharedPtr &&) = delete;
    
 	// �洢��ָ�룬ʹ��release�ڴ�˳�򣬱�֤����д����������洢����֮ǰ���
    void store(std::shared_ptr<T> ptr) noexcept {
      std::atomic_store_explicit(&_ptr, ptr, std::memory_order_release);
    }
 	
    void reset(std::shared_ptr<T> ptr = nullptr) noexcept {
      store(ptr);
    }
    
 	// ����ָ��ĵ�ǰֵ��ʹ��acquire�ڴ�˳��
    std::shared_ptr<T> load() const noexcept {
      return std::atomic_load_explicit(&_ptr, std::memory_order_acquire);
    }
 	
 	// ԭ�ӱȽϲ����������������ǰָ��ֵ��Ԥ��ֵƥ�䣬���滻Ϊdesiredֵ
    bool compare_exchange(std::shared_ptr<T> *expected, std::shared_ptr<T> desired) noexcept {
      return std::atomic_compare_exchange_strong_explicit(
          &_ptr,
          expected,
          desired,
          std::memory_order_acq_rel,
          std::memory_order_acq_rel);
    }
 
    AtomicSharedPtr &operator=(std::shared_ptr<T> ptr) noexcept {
      store(std::move(ptr));
      return *this;
    }
 
    AtomicSharedPtr &operator=(const AtomicSharedPtr &rhs) noexcept {
      store(rhs.load());
      return *this;
    }
 
    AtomicSharedPtr &operator=(AtomicSharedPtr &&) = delete;
 
  private:
 
    std::shared_ptr<T> _ptr;
  };
 
} // namespace carla
