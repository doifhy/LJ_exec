#include"group_varint_encode.h"
#include<bitset>
#include<iostream>


//分组进行encode
//每组数据数量(1 <= n <= 4)
string encode_single_group(const vector<uint32_t>& integers){	
	string encoded;	
	//该组的tag	
	char tags = 0;
	size_t offset = 6;
	//对于每一个数据进行encode
	for(int i = 0; i < integers.size(); i++){
		uint32_t number = integers[i];
		//每个数据有一个子标签count记录需要多少个Byte
		char count = 0x0;
		//为0则不需要取，直接添加至encoded
		if(number != 0){
			char current_byte = 0xFF;
			//当还有字节可以编码时：取出字节-》计数增加-》加入编码
			while(number != 0){
				current_byte = current_byte & number;
				count++;
				encoded += current_byte;
				number = number >> 8;
				current_byte = 0xFF;	
			}
			tags = tags | (count-1 << offset);
		}else{
			encoded += (char)0x00;
			tags = tags | (count << offset);	
		}
		offset -= 2;
	}
	return (tags + encoded);
}

//对未编码数组进行group_varint_encode
//return encoded string
string group_varint_encode(const vector<uint32_t> &original_integers){
	//为空返回空字符串	
	if(original_integers.empty()) return "";
	string encoded;
	//分组器
	int group_count = 0;
	vector<uint32_t> group;
	for(int i = 0; i < original_integers.size(); i++){
		group_count++;
		group.push_back(original_integers[i]);
		//若能填满四个数据则编码
		if((group_count & 3) == 0){
			encoded += encode_single_group(group);
			group_count = 0;
			group.clear();
		}
	}
	//若数据数量不为4的倍数则再单独编码
	if(group_count != 0) encoded += encode_single_group(group);
	return encoded;
}

uint32_t get_value(const string& encoded_byte_stream, const vector<uint32_t>& masks, size_t current_tag, uint32_t* address, int& index){
	uint32_t value = 0;	
	address = (uint32_t*)&encoded_byte_stream[index];
	value = *address & masks[current_tag];
	index += current_tag + 1;
	return value;

}
//decode操作，返回uint32_t的vector
vector<uint32_t> group_varint_decode(const string& encoded_byte_stream){
	if(encoded_byte_stream.empty()) return {};
	vector<uint32_t> decoded;	
	const int len = encoded_byte_stream.length();
	int index = 0;
	const vector<uint32_t> masks = {0xff, 0xffff, 0xffffff, 0xffffffff};
	while(index < len){
		//取当前tag
		char tags = encoded_byte_stream[index++];
		int number = (tags & 3) + ((tags >> 2) & 3) + ((tags >> 4) & 3) + ((tags >> 6) & 3) + 4;
		uint32_t* address = (uint32_t*)&encoded_byte_stream[index];
		size_t current_tag = (tags >> 6) & 3; 
		//最后一个		
		if((index + number) >= len){
			decoded.push_back(get_value(encoded_byte_stream,masks, current_tag, address, index));
			if(index >= len) break;
			
			current_tag = (tags >> 4) & 3;
			decoded.push_back(get_value(encoded_byte_stream,masks, current_tag, address, index));
			if(index >= len) break;

			current_tag = (tags >> 2) & 3;
			decoded.push_back(get_value(encoded_byte_stream,masks, current_tag, address, index));
			if(index >= len) break;
		
			current_tag = tags & 3;
			decoded.push_back(get_value(encoded_byte_stream,masks, current_tag, address, index));
			if(index >= len) break;

		}else{
					
			decoded.push_back(get_value(encoded_byte_stream,masks, current_tag, address, index));

			current_tag = (tags >> 4) & 3;
			decoded.push_back(get_value(encoded_byte_stream,masks, current_tag, address, index));

			current_tag = (tags >> 2) & 3;
			decoded.push_back(get_value(encoded_byte_stream,masks, current_tag, address, index));

			current_tag = tags & 3;
			decoded.push_back(get_value(encoded_byte_stream,masks, current_tag, address, index));
		}
	}		
	return decoded;
}
