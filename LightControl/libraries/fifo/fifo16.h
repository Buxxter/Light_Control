#ifndef FIFO16_H_
#define FIFO16_H_

//размер должен быть степенью двойки: 4,8,16,32...128
#define FIFO16( size )\
struct {\
	uint16_t buf[size];\
	unsigned char tail;\
	unsigned char head;\
}

//количество элементов в очереди
#define FIFO16_COUNT(fifo)     (fifo.head-fifo.tail)

//размер fifo
#define FIFO16_SIZE(fifo)      ( sizeof(fifo.buf)/sizeof(fifo.buf[0]) )

//fifo заполнено?
#define FIFO16_IS_FULL(fifo)   (FIFO16_COUNT(fifo)==FIFO16_SIZE(fifo))

//fifo пусто?
#define FIFO16_IS_EMPTY(fifo)  (fifo.tail==fifo.head)

//количество свободного места в fifo
#define FIFO16_SPACE(fifo)     (FIFO16_SIZE(fifo)-FIFO16_COUNT(fifo))

//поместить элемент в fifo
#define FIFO16_PUT(fifo, byte) \
{\
	fifo.buf[fifo.head & (FIFO16_SIZE(fifo)-1)]=byte;\
	fifo.head++;\
}

//взять первый элемент из fifo
#define FIFO16_PEEK(fifo) (fifo.buf[fifo.tail & (FIFO16_SIZE(fifo)-1)])

//взять последний элемент из fifo
#define FIFO16_PEEK_LAST(fifo) (fifo.buf[(fifo.head - 1) & (FIFO16_SIZE(fifo)-1)])

//удалить последний элемент из fifo
#define FIFO16_REMOVE_LAST(fifo) {if (fifo.tail!=fifo.head) {fifo.head--;}}

//уменьшить количество элементов в очереди
#define FIFO16_POP(fifo)   \
{\
	fifo.tail++; \
}

#define FIFO16_GET(fifo) (fifo.buf[fifo.tail++ & (FIFO16_SIZE(fifo)-1)])

//очистить fifo
#define FIFO16_FLUSH(fifo)   \
{\
	fifo.tail=0;\
	fifo.head=0;\
}



#endif /* FIFO16_H_ */