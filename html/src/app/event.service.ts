import {EventEmitter, Injectable} from "@angular/core";

export const ROI_SET = 'ROI_SET';

@Injectable()
export class EventService {

    private events: EventEmitter<any> = new EventEmitter<any>();

    emit(event: string, data?: any) {
        this.events.emit({ name: event, data: data });
    }

    subscribe(callback: any) {
        return this.events.subscribe(callback);
    }

}
