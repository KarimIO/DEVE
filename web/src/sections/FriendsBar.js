import React, { Component } from 'react';

export default class FriendsBar extends Component {

    getList = () => {
        let list = this.props.list;
        return list.map((e, i) => {
            return <div className={(this.props.sel === e.username) ? "selected" : ""} onClick={() => this.props.setUser(e.username)} key={i}>
                {e.name}
            </div> 
        });
    }
    
	render() {
        let empty = this.props.list.length === 0;
		return (
			<aside>
                <div className="logo"></div>
                <div className={`main-section user-container${this.props.loading ? " loading" : ""}${empty ? " empty":""}`}>
                    <span className="badge">Welcome, {this.props.name}</span>
                    {this.getList()}
                </div>
            </aside>
		);
	}
}