import React, { Component } from 'react';

export default class FriendsBar extends Component {

    getList = () => {
        let list = [
            {n:"Ahmed Ghazy", u:"ahmedghazy"},
            {n:"Ali Khaled", u:"alikhaled"},
            {n:"Amr El Kadi", u:"amrkadi"},
            {n:"Karim Abdel Hamid", u:"karimah"},
            {n:"Mohamed Gaber", u:"donn"}
        ]

        return list.map((e, i) => {
            return <div className={(this.props.sel === e.u) ? "selected" : ""} onClick={() => this.props.setUser(e.u)} key={i}>{e.n}</div> 
        });
    }
    
	render() {
		return (
			<aside>
                <div className="logo"></div>
                <div className="friends-container">
                    {this.getList()}
                </div>
            </aside>
		);
	}
}