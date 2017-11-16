import {Component} from "@angular/core";
import {ObserversStateMessagesService, ObserverState} from "../observers_state_messages.service";

@Component({
    selector: 'observers-state-messages',
    template: require('./observers-state-messages.html'),
    styles: [require('./observers-state-messages.scss')]
})
export class ObserversStateMessagesComponent {
    private observersStateMessages: ObserverState[][] = [];

    constructor(private logService: ObserversStateMessagesService) {
        logService.getLog().subscribe(observersStateMessages => {
            if (observersStateMessages && observersStateMessages.length > 0) {
                console.log(observersStateMessages);
                this.observersStateMessages.push(observersStateMessages);
            }
        });
    }
}